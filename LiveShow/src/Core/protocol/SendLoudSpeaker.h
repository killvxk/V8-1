#pragma once
#include "..\..\CommonLibrary\include\net\packet.h"
#include "ProtocolBase.h"
#include "ProtocolDefine.h"
#include "coredefine.h"

class SendLoudSpeakerPacket : public ClientPacket
{
public:
	SendLoudSpeakerPacket(core::SendLoudSpeakerEvent& _event)
		:ClientPacket(ROOM_SEND_LOUDSPEAKER_REQ, _event.unRoomID),stEvent(_event){};

	void Pack();

private:
	core::SendLoudSpeakerEvent& stEvent;
};

//////////////////////////////////////////////////////////////////////////

class SendLoudSpeakerRspPacket : public ServerPacket
{
public:
	SendLoudSpeakerRspPacket(ByteBuf & buf, core::SendLoudSpeakerRspEvent& _event, uint32 unseq)
		:ServerPacket(buf), stEvent(_event), seq(unseq){};

	void Unpack();

private:
	core::SendLoudSpeakerRspEvent& stEvent;
	uint32 seq;
};

//////////////////////////////////////////////////////////////////////////

class SendLoudSpeakerNotifyPacket : public ServerPacket
{
public:
	SendLoudSpeakerNotifyPacket(ByteBuf & buf, core::SendLoudSpeakerNotifyEvent& _event)
		:ServerPacket(buf), stEvent(_event){};

	void Unpack();

private:
	core::SendLoudSpeakerNotifyEvent& stEvent;
};