// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/stats_table.h"

#include "base/string_util.h"
#include "base/logging.h"
#include "base/thread_local_storage.h"
#include "base/platform_thread.h"

#if defined(OS_POSIX)
#include "errno.h"
#endif

// The StatsTable uses a shared memory segment that is laid out as follows
//
// +-------------------------------------------+
// | Version | Size | MaxCounters | MaxThreads |
// +-------------------------------------------+
// | Thread names table                        |
// +-------------------------------------------+
// | Thread TID table                          |
// +-------------------------------------------+
// | Thread PID table                          |
// +-------------------------------------------+
// | Counter names table                       |
// +-------------------------------------------+
// | Data                                      |
// +-------------------------------------------+
//
// The data layout is a grid, where the columns are the thread_ids and the
// rows are the counter_ids.
//
// If the first character of the thread_name is '\0', then that column is
// empty.
// If the first character of the counter_name is '\0', then that row is
// empty.
//
// About Locking:
// This class is designed to be both multi-thread and multi-process safe.
// Aside from initialization, this is done by partitioning the data which
// each thread uses so that no locking is required.  However, to allocate
// the rows and columns of the table to particular threads, locking is
// required.
//
// At the shared-memory level, we have a lock.  This lock protects the
// shared-memory table only, and is used when we create new counters (e.g.
// use rows) or when we register new threads (e.g. use columns).  Reading
// data from the table does not require any locking at the shared memory
// level.
//
// Each process which accesses the table will create a StatsTable object.
// The StatsTable maintains a hash table of the existing counters in the
// table for faster lookup.  Since the hash table is process specific,
// each process maintains its own cache.  We avoid complexity here by never
// de-allocating from the hash table.  (Counters are dynamically added,
// but not dynamically removed).

// In order for external viewers to be able to read our shared memory,
// we all need to use the same size ints.
COMPILE_ASSERT(sizeof(int)==4, expect_4_byte_ints);

namespace {

// An internal version in case we ever change the format of this
// file, and so that we can identify our table.
const int kTableVersion = 0x13131313;

// The name for un-named counters and threads in the table.
const wchar_t kUnknownName[] = L"<unknown>";

// Calculates delta to align an offset to the size of an int
inline int AlignOffset(int offset) {
  return (sizeof(int) - (offset % sizeof(int))) % sizeof(int);
}

inline int AlignedSize(int size) {
  return size + AlignOffset(size);
}

// StatsTableTLSData carries the data stored in the TLS slots for the
// StatsTable.  This is used so that we can properly cleanup when the
// thread exits and return the table slot.
//
// Each thread that calls RegisterThread in the StatsTable will have
// a StatsTableTLSData stored in its TLS.
struct StatsTableTLSData {
  StatsTable* table;
  int slot;
};

// The SlotReturnFunction is called at thread exit for each thread
// which used the StatsTable.
static void SlotReturnFunction(void* data) {
  StatsTableTLSData* tls_data = static_cast<StatsTableTLSData*>(data);
  if (tls_data) {
    DCHECK(tls_data->table);
    tls_data->table->UnregisterThread();
  }
}

}  // namespace

// The StatsTablePrivate maintains convenience pointers into the
// shared memory segment.  Use this class to keep the data structure
// clean and accessible.
class StatsTablePrivate {
 public:
  // Various header information contained in the memory mapped segment.
  struct TableHeader {
    int version;
    int size;
    int max_counters;
    int max_threads;
  };

  // Create the StatsTablePrivate based on expected size parameters.
  StatsTablePrivate(void* memory, int size, int max_threads, int max_counters);

  // Accessors for our header pointers
  TableHeader* table_header() const { return table_header_; }
  int version() const { return table_header_->version; }
  int size() const { return table_header_->size; }
  int max_counters() const { return table_header_->max_counters; }
  int max_threads() const { return table_header_->max_threads; }

  // Accessors for our tables
  wchar_t* thread_name(int slot_id) const {
    return &thread_names_table_[
      (slot_id-1) * (StatsTable::kMaxThreadNameLength)];
  }
  int* thread_tid(int slot_id) const {
    return &(thread_tid_table_[slot_id-1]);
  }
  int* thread_pid(int slot_id) const {
    return &(thread_pid_table_[slot_id-1]);
  }
  wchar_t* counter_name(int counter_id) const {
    return &counter_names_table_[
      (counter_id-1) * (StatsTable::kMaxCounterNameLength)];
  }
  int* row(int counter_id) const {
    return &data_table_[(counter_id-1) * max_threads()];
  }

 private:
  // Initializes the table on first access.  Sets header values
  // appropriately and zeroes all counters.
  void InitializeTable(void* memory, int size, int max_counters,
                       int max_threads);

  // Initializes our in-memory pointers into a pre-created StatsTable.
  void ComputeMappedPointers(void* memory);

  TableHeader* table_header_;
  wchar_t* thread_names_table_;
  int* thread_tid_table_;
  int* thread_pid_table_;
  wchar_t* counter_names_table_;
  int* data_table_;
};

StatsTablePrivate::StatsTablePrivate(void* memory, int size, int max_threads,
                                     int max_counters) {
  TableHeader* header = static_cast<TableHeader*>(memory);
  // If the version does not match, then assume the table needs
  // to be initialized.
  if (header->version != kTableVersion)
    InitializeTable(memory, size, max_counters, max_threads);

  // We have a valid table, so compute our pointers.
  ComputeMappedPointers(memory);
}

void StatsTablePrivate::InitializeTable(void* memory, int size,
                                        int max_counters,
                                        int max_threads) {
  // Zero everything.
  memset(memory, 0, size);

  // Initialize the header.
  TableHeader* header = static_cast<TableHeader*>(memory);
  header->version = kTableVersion;
  header->size = size;
  header->max_counters = max_counters;
  header->max_threads = max_threads;
}

void StatsTablePrivate::ComputeMappedPointers(void* memory) {
  char* data = static_cast<char*>(memory);
  int offset = 0;

  table_header_ = reinterpret_cast<TableHeader*>(data);
  offset += sizeof(*table_header_);
  offset += AlignOffset(offset);

  // Verify we're looking at a valid StatsTable.
  DCHECK_EQ(table_header_->version, kTableVersion);

  thread_names_table_ = reinterpret_cast<wchar_t*>(data + offset);
  offset += sizeof(wchar_t) *
            max_threads() * StatsTable::kMaxThreadNameLength;
  offset += AlignOffset(offset);

  thread_tid_table_ = reinterpret_cast<int*>(data + offset);
  offset += sizeof(int) * max_threads();
  offset += AlignOffset(offset);

  thread_pid_table_ = reinterpret_cast<int*>(data + offset);
  offset += sizeof(int) * max_threads();
  offset += AlignOffset(offset);

  counter_names_table_ = reinterpret_cast<wchar_t*>(data + offset);
  offset += sizeof(wchar_t) *
            max_counters() * StatsTable::kMaxCounterNameLength;
  offset += AlignOffset(offset);

  data_table_ = reinterpret_cast<int*>(data + offset);
  offset += sizeof(int) * max_threads() * max_counters();

  DCHECK_EQ(offset, size());
}



// We keep a singleton table which can be easily accessed.
StatsTable* StatsTable::global_table_ = NULL;

StatsTable::StatsTable(const std::wstring& name, int max_threads,
                       int max_counters)
    : tls_index_(SlotReturnFunction) {
  int table_size =
    AlignedSize(sizeof(StatsTablePrivate::TableHeader)) +
    AlignedSize((max_counters * sizeof(wchar_t) * kMaxCounterNameLength)) +
    AlignedSize((max_threads * sizeof(wchar_t) * kMaxThreadNameLength)) +
    AlignedSize(max_threads * sizeof(int)) +
    AlignedSize(max_threads * sizeof(int)) +
    AlignedSize((sizeof(int) * (max_counters * max_threads)));

  impl_ = NULL;
  // TODO(mbelshe): Move this out of the constructor
  if (shared_memory_.Create(name, false, true, table_size))
    if (shared_memory_.Map(table_size))
      impl_ = new StatsTablePrivate(shared_memory_.memory(), table_size,
                                    max_threads, max_counters);
#if defined(OS_WIN)
  if (!impl_)
    LOG(ERROR) << "StatsTable did not initialize:" << GetLastError();
#elif defined(OS_POSIX)
  if (!impl_)
    LOG(ERROR) << "StatsTable did not initialize:" << strerror(errno);
#endif
}

StatsTable::~StatsTable() {
  // Before we tear down our copy of the table, be sure to
  // unregister our thread.
  UnregisterThread();

  // Return ThreadLocalStorage.  At this point, if any registered threads
  // still exist, they cannot Unregister.
  tls_index_.Free();

  // Cleanup our shared memory.
  delete impl_;

  // If we are the global table, unregister ourselves.
  if (global_table_ == this)
    global_table_ = NULL;
}

int StatsTable::RegisterThread(const std::wstring& name) {
  int slot = 0;

  // Registering a thread requires that we lock the shared memory
  // so that two threads don't grab the same slot.  Fortunately,
  // thread creation shouldn't happen in inner loops.
  {
    SharedMemoryAutoLock lock(&shared_memory_);
    slot = FindEmptyThread();
    if (!slot) {
      return 0;
    }

    DCHECK(impl_);

    // We have space, so consume a column in the table.
    std::wstring thread_name = name;
    if (name.empty())
      thread_name = kUnknownName;
    base::wcslcpy(impl_->thread_name(slot), thread_name.c_str(),
                  kMaxThreadNameLength);
    *(impl_->thread_tid(slot)) = PlatformThread::CurrentId();
    // TODO(pinkerton): these should go into process_utils when it's ported
#if defined(OS_WIN)
    *(impl_->thread_pid(slot)) = GetCurrentProcessId();
#elif defined(OS_POSIX)
    *(impl_->thread_pid(slot)) = getpid();
#endif  
  }

  // Set our thread local storage.
  StatsTableTLSData* data = new StatsTableTLSData;
  data->table = this;
  data->slot = slot;
  tls_index_.Set(data);
  return slot;
}

StatsTableTLSData* StatsTable::GetTLSData() const {
  StatsTableTLSData* data =
    static_cast<StatsTableTLSData*>(tls_index_.Get());
  if (!data)
    return NULL;

  DCHECK(data->slot);
  DCHECK_EQ(data->table, this);
  return data;
}

void StatsTable::UnregisterThread() {
  StatsTableTLSData* data = GetTLSData();
  if (!data)
    return;
  DCHECK(impl_);

  // Mark the slot free by zeroing out the thread name.
  wchar_t* name = impl_->thread_name(data->slot);
  *name = L'\0';

  // Remove the calling thread's TLS so that it cannot use the slot.
  tls_index_.Set(NULL);
  delete data;
}

int StatsTable::CountThreadsRegistered() const {
  if (!impl_)
    return 0;

  // Loop through the shared memory and count the threads that are active.
  // We intentionally do not lock the table during the operation.
  int count = 0;
  for (int index = 1; index <= impl_->max_threads(); index++) {
    wchar_t* name = impl_->thread_name(index);
    if (*name != L'\0')
      count++;
  }
  return count;
}

int StatsTable::GetSlot() const {
  StatsTableTLSData* data = GetTLSData();
  if (!data)
    return 0;
  return data->slot;
}

int StatsTable::FindEmptyThread() const {
  // Note: the API returns slots numbered from 1..N, although
  // internally, the array is 0..N-1.  This is so that we can return
  // zero as "not found".
  //
  // The reason for doing this is because the thread 'slot' is stored
  // in TLS, which is always initialized to zero, not -1.  If 0 were
  // returned as a valid slot number, it would be confused with the
  // uninitialized state.
  if (!impl_)
    return 0;

  int index = 1;
  for (; index <= impl_->max_threads(); index++) {
    wchar_t* name = impl_->thread_name(index);
    if (!*name)
      break;
  }
  if (index > impl_->max_threads())
    return 0;  // The table is full.
  return index;
}

int StatsTable::FindCounterOrEmptyRow(const std::wstring& name) const {
  // Note: the API returns slots numbered from 1..N, although
  // internally, the array is 0..N-1.  This is so that we can return
  // zero as "not found".
  //
  // There isn't much reason for this other than to be consistent
  // with the way we track columns for thread slots.  (See comments
  // in FindEmptyThread for why it is done this way).
  if (!impl_)
    return 0;

  int free_slot = 0;
  for (int index = 1; index <= impl_->max_counters(); index++) {
    wchar_t* row_name = impl_->counter_name(index);
    if (!*row_name && !free_slot)
      free_slot = index;  // save that we found a free slot
    else if (!wcsncmp(row_name, name.c_str(), kMaxCounterNameLength))
      return index;
  }
  return free_slot;
}

int StatsTable::FindCounter(const std::wstring& name) {
  // Note: the API returns counters numbered from 1..N, although
  // internally, the array is 0..N-1.  This is so that we can return
  // zero as "not found".
  if (!impl_)
    return 0;

  // Create a scope for our auto-lock.
  {
    AutoLock scoped_lock(counters_lock_);

    // Attempt to find the counter.
    CountersMap::const_iterator iter;
    iter = counters_.find(name);
    if (iter != counters_.end())
      return iter->second;
  }

  // Counter does not exist, so add it.
  return AddCounter(name);
}

int StatsTable::AddCounter(const std::wstring& name) {
  DCHECK(impl_);

  if (!impl_)
    return 0;

  int counter_id = 0;
  {
    // To add a counter to the shared memory, we need the
    // shared memory lock.
    SharedMemoryAutoLock lock(&shared_memory_);

    // We have space, so create a new counter.
    counter_id = FindCounterOrEmptyRow(name);
    if (!counter_id)
      return 0;

    std::wstring counter_name = name;
    if (name.empty())
      counter_name = kUnknownName;
    base::wcslcpy(impl_->counter_name(counter_id), counter_name.c_str(),
                  kMaxCounterNameLength);
  }

  // now add to our in-memory cache
  {
    AutoLock lock(counters_lock_);
    counters_[name] = counter_id;
  }
  return counter_id;
}

int* StatsTable::GetLocation(int counter_id, int slot_id) const {
  if (!impl_)
    return NULL;
  if (slot_id > impl_->max_threads())
    return NULL;

  int* row = impl_->row(counter_id);
  return &(row[slot_id-1]);
}

const wchar_t* StatsTable::GetRowName(int index) const {
  if (!impl_)
    return NULL;

  return impl_->counter_name(index);
}

int StatsTable::GetRowValue(int index, int pid) const {
  if (!impl_)
    return 0;

  int rv = 0;
  int* row = impl_->row(index);
  for (int index = 0; index < impl_->max_threads(); index++) {
    if (pid == 0 || *impl_->thread_pid(index) == pid)
      rv += row[index];
  }
  return rv;
}

int StatsTable::GetRowValue(int index) const {
  return GetRowValue(index, 0);
}

int StatsTable::GetCounterValue(const std::wstring& name, int pid) {
  if (!impl_)
    return 0;

  int row = FindCounter(name);
  if (!row)
    return 0;
  return GetRowValue(row, pid);
}

int StatsTable::GetCounterValue(const std::wstring& name) {
  return GetCounterValue(name, 0);
}

int StatsTable::GetMaxCounters() const {
  if (!impl_)
    return 0;
  return impl_->max_counters();
}

int StatsTable::GetMaxThreads() const {
  if (!impl_)
    return 0;
  return impl_->max_threads();
}

int* StatsTable::FindLocation(const wchar_t* name) {
  // Get the static StatsTable
  StatsTable *table = StatsTable::current();
  if (!table)
    return NULL;

  // Get the slot for this thread.  Try to register
  // it if none exists.
  int slot = table->GetSlot();
  if (!slot && !(slot = table->RegisterThread(L"")))
      return NULL;

  // Find the counter id for the counter.
  std::wstring str_name(name);
  int counter = table->FindCounter(str_name);

  // Now we can find the location in the table.
  return table->GetLocation(counter, slot);
}

