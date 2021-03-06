#pragma once
#include "..\..\CommonLibrary\include\net\packet.h"
#include "ProtocolBase.h"
#include "ProtocolDefine.h"
#include "coredefine.h"

class ReportClientInfoPacket : public ClientPacket
{
public:
	ReportClientInfoPacket(core::ReportClientInfoEvent& _event)
		:ClientPacket(ROOM_REPORT_CLIENTINFO_REQ, _event.unRoomID),stEvent(_event){};

	void Pack();

	enum ENM_ReportType
	{
		REPORTTYPE_VIDEO	=	0x00,//+havevideo(uint8)
		REPORTTYPE_NETSPEED	=	0x01,//+packet_lost(uint8)+ping_timeout(uint8)
		REPORTTYPE_HARDWARE	=	0x02,//+cputype(string)+cpuuse(uint16)+memsize(uint16)+memuse(uint16)
	};


private:
	core::ReportClientInfoEvent& stEvent;
};

//////////////////////////////////////////////////////////////////////////

class ReportClientInfoNotifyPacket : public ServerPacket
{
public:
	ReportClientInfoNotifyPacket(ByteBuf & buf, core::ReportClientInfoNotifyEvent& _event)
		:ServerPacket(buf), stEvent(_event){};

	void Unpack();

private:
	core::ReportClientInfoNotifyEvent& stEvent;
};

//////////////////////////////////////////////////////////////////////////