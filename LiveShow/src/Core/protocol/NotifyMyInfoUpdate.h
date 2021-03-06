#pragma once
#include "..\..\CommonLibrary\include\net\packet.h"
#include "ProtocolBase.h"
#include "ProtocolDefine.h"
#include "coredefine.h"

class NotifyMyInfoUpdatePacket : public ServerPacket
{
public:
	NotifyMyInfoUpdatePacket(ByteBuf & buf):ServerPacket(buf){}

	void Unpack()
	{
		ServerPacket::Unpack();

		uint8 unMan;
		*this>>str179Name>>unMan;
		bMan = unMan;
	}

	std::wstring str179Name;
	bool bMan;
};