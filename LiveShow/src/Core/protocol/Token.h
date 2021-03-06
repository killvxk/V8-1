#pragma once
#include "..\..\CommonLibrary\include\net\packet.h"

struct TokenProtocol
{
	enum TokenConst
	{
		MAX_CLIENT_MSG_LEN = 2048,
		MAX_SERVER_MSG_LEN = 2048,
		MAX_HEAD_OPTION_LEN = 128,
		MAX_ERRMSG_LEN = 256,
		MAX_UID_LEN = 64,
		MAX_IP_LEN = 32,
		MAX_SOURCE_LEN = 64,
		MAX_PSWD_LEN = 64,
		MAX_URL_LEN = 256,
	};

	struct PkgHead
	{
		char tag;
		short totallen;
		short headlen;
		short msgid;
		short version;
		char flag[2];
		short optionlen;
		char option[TokenProtocol::MAX_HEAD_OPTION_LEN];
	};

	struct AuthReq
	{
		char uidlen;
		char uid[TokenProtocol::MAX_UID_LEN];
		char isfirst;
		char iplen;
		char ip[TokenProtocol::MAX_IP_LEN];
		char sourcelen;
		char source[TokenProtocol::MAX_SOURCE_LEN];
		short appid;
		short passwordlen;
		char password[TokenProtocol::MAX_PSWD_LEN];
	};

	enum TokenRspError
	{
		TOKEN_RSP_NO_HEAD,
		TOKEN_RSP_NO_ENOUGH_LENTH,
		TOKEN_RSP_SUCCESS,
		TOKEN_RSP_ERROR,
		TOKEN_RSP_UNKNOWN_ERROR,
	};

	enum ErrorCode
	{
		COMMON_ERROR=0x0001	,//通用错误
		ACCOUNT_NOEXIST=0x0002	,//用户不存在
		PASSWORD_WRONG=0x0003	,//密码错误
		ACCOUNT_FORBIDEN=0x0004	,//用户被禁用
		JUREN_ACCOUNT_NOEXIST=0x0008	,//巨人用户不存在
		JUREN_PASSWORD_WRONG=0x0009	,//巨人密码错误
		NEED_VERIFY=0x000e,//登陆限制，需要验证码验证
		VERIFY_FORMAT_ERROR=0x000f,//验证码验证失败
		VERIFY_ERROR=0x0010,//验证码验证失败
		VERIFY_TIMEOUT=0x0011,//验证码超时
		LOGIN_REFUSED=0x0012,//登陆太频繁，账号被锁定(10分钟内最多登陆20次)
		LOGIN_INVALID_EMAIL_PASS_SUCC=0x0013,//邮箱注册的用户没有激活，密码正确
		LOGIN_INVALID_EMAIL_PASS_FAIL=0x0014,//邮箱注册的用户没有激活，密码错误
		EVERYTHING_OK,//客户端定义，使用
	};

	static char reqbuf[TokenProtocol::MAX_CLIENT_MSG_LEN];
	static ErrorCode errorCode;
	static std::wstring errorMsg;
	static std::wstring errorUrl;
	static std::wstring errorMsg2;

	static void GenOption(PkgHead& req_head, char arrbyKey[16], std::string& strVerifyKey, std::wstring& strVerifyCode);

	static ByteBuf GenTokenReq(std::string const& username,std::string const& pass,bool beCylcle,bool b179,
		bool bVerify = false, std::string strVerifyKey = "", std::wstring strVerifyCode = L"");

	static TokenRspError Decode51TokenRsp(uint32& unOutUIN, std::string const &pass,
		ByteBuf &,ByteBuf &,ByteBuf &,uint32 &, ByteBuf &, ByteBuf &);

	static TokenRspError Decode179TokenRsp(uint32& unOutUIN, std::string const &pass,
		ByteBuf &,ByteBuf &,ByteBuf &,uint32 &, ByteBuf &);
};