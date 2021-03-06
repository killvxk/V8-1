#pragma once
#include "net\packet.h"
#include "..\ProtocolBase.h"
#include "..\ProtocolDefine.h"
#include "coredefine.h"

class GetUserListPacket : public ClientPacket
{
public:
	GetUserListPacket(uint32 roomid):ClientPacket(ROOM_GET_USERLIST_REQ, roomid){};

	void Pack()
	{
		HeadBusiness();
		TailBusiness();
	}
};

//////////////////////////////////////////////////////////////////////////

class GetUserListRspPacket : public ServerPacket
{
public:
	GetUserListRspPacket(ByteBuf & buf, core::GetUserListRspEvent& _event)
		:ServerPacket(buf), stEvent(_event){};

	void Unpack();

private:
	core::GetUserListRspEvent& stEvent;
};