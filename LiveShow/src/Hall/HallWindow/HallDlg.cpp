// LobbyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "..\Hall.h"
#include "HallDlg.h"
#include "skin_i.h"
#include "SkinUtil.h"
#include "VividTree.h"
#include "..\LoginWindow\LoginDlg.h"
#include "SearchDlg.h"
#include "..\AboutDlg.h"
#include "..\CommonLibrary\include\ui\C179MsgBox.h"
#include "roomdefine.h"
#include "../SysSettingIni.h"
#include "..\CommonLibrary\include\ui\MenuEx.h"
#include "..\CommonLibrary\include\ui\UIHelper.h"
#include "..\CommonLibrary\include\utility\SystemHelper.h"
#include "HallDefine.h" 
#include "EnterRoomRequest.h"
#include "webdefine.h"
#include "..\SysNotify\SysNotifyDlg.h"
#include "..\HeadSetting\HeadSettingDlg.h"
#include "..\CommonDll\Common.h"
#include "..\LoginWindow\179TipCtrl.h"
#include "..\CommonLibrary\include\utility\stringhelper.h"
#include "../CommonLibrary/include/utility/json/json.h"
#include "../CommonLibrary/include/utility/ConfigManager.h"
#include "utility/URLEncode.h"


// CLobbyDlg 对话框

#define SKIN_ON_CLICK_MINIMIZE                       2
#define SKIN_ON_CLICK_MAXIMIZE                       3
#define SKIN_ON_CLICK_CLOSE                          4
//#define SKIN_ON_SPLITTER_MOVE                        5
//#define SKIN_NOTIFY_SPLITTER_MOVE                    6
#define SKIN_ON_DIALOG_RESIZE                        7
#define SKIN_NOTIFY_SEARCH_DLG_HWND                  8
#define SKIN_ON_CLICK_HEAD                          12
#define SKIN_ON_CLICK_SYSSETTING                    14
#define SKIN_ON_CLICK_MGRCENTER                     15
#define SKIN_ON_CLICK_LOBBY_HOME                    16
#define SKIN_ON_CLICK_ACCOUNT_PAY                   17
#define SKIN_ON_CLICK_RANK                          18
#define SKIN_ON_CLICK_OFFICIAL_FORUM                19
#define SKIN_ON_CLICK_REFRESH                       20
//#define SKIN_ON_CLICK_CHANGE_LINE                   21
#define SKIN_ON_CLICK_HOME_PAGE                     22
#define SKIN_ON_CLICK_USER_HELP                     24
#define SKIN_ON_CLICK_VIP                           25
#define SKIN_ON_CLICK_BTN_BACK                      26
#define SKIN_NOTIFY_IS_GUEST                        27
#define SKIN_NOTIFY_NO_GUEST                        28
#define SKIN_ON_CLICK_BTN_LOGIN                     29//游客登录时可以点登录和注册
#define SKIN_ON_CLICK_BTN_REGISTER                  30
#define SKIN_ON_CLICK_BTN_SAFESETTING				31 	//安全中心
//#define SKIN_NOTIFY_MOVE_TREE_CTRL                  32  
#define SKIN_ON_CLICK_SYS_FEEDBACK					33

#define TREE_HALLINFO	0	//房间目录树
#define TREE_RESULTINFO 2	//搜索结果

#define DLG_WIDTH 579
#define DLG_HEIGHT 500

#define SEARCH_NOFIND_KEYTRUE  _T("抱歉，您查找的房间不存在")
#define SEARCE_NOFIND_KEYFALSE  _T("至少输入5个字符")

#define TIKCER_RESULTS_CONTENT "content"   //跑马灯某一条数据
#define TIKCER_RESULTS_URL "url"          //跑马灯某一条数据对应的url

#define RICHLEVEL_CONFIG_PATH				L"resource\\rich\\richconfig.xml"
#define RICHLEVEL_DIR						L"resource\\rich\\"
#define RESOUCRE_LISTCION					_T("resource\\listicon\\")
#define RESOUCRE_HEAD_PATH					_T("resource\\head\\")


#define HALLTREE_TIMER_IDEVENT 4     //定时器 大厅目录树定时更新
#define HALLTREE_TIMER_INTERVALTIME  5*60*1000  //定时器间隔 大厅目录树定时更新间隔时间（单位：豪秒，即5分钟）
#define HALLSAFESETTING_TIMER_TRAY	6  //定时器 安全中心
#define HALLSAFESETTING_TIMER_INTERVALTIME	1000	//定时器间隔
#define HALL_TICKER_TIMER_IDEVENT 7  //定时器 大厅底部跑马灯定时获取最新数据
#define HALL_TICKER_TIMER_INTERVALTIME 15*60*1000  //定时器间隔  大厅底部跑马灯定时获取最新数据间隔时间（单位：豪秒，即15分钟）
#define PROCESS_REPORT_TIMER_IDEVENT 8   //定时器 对手进程定时查找上传
#define PROCESS_REPORT_TIMER_INTERVALTIME  10*60*1000 //定时器间隔  对手进程定时查找上传的间隔时间（单位：豪秒，即10分钟）

IMPLEMENT_DYNAMIC(CHallDlg, CDialog)

CHallDlg::CHallDlg(IModuleManager* pModuleManager,CWnd* pParent /*=NULL*/)
: CDialog(CHallDlg::IDD, pParent)
{
	m_pModuleManager = NULL;
	m_pModuleManager = pModuleManager;
	m_pSearchDlg = NULL;	
	m_pSkinEngine = NULL;
	m_showType = TREE_HALLINFO;
	m_nHallTeeSeqNo= 0;
	m_pChangeExitDlg = NULL;
	m_RegisterDlg = NULL;
	m_GameDlg = NULL;
	::GetSkinEngine(&m_pSkinEngine);
	m_bGraySafeSetting = TRUE ;
	m_pTipCtrl = NULL ;
	m_bSearchResult = FALSE;
	m_TickerSeqNo = 0;	
	m_pRichLevelBtn = NULL;
	m_htmlIsLoadComplete = false;
	phtmlRightResult=NULL;
}

CHallDlg::~CHallDlg()
{
	m_htmlRight.DestroyWindow();
	m_treeCtrl.DestroyWindow();

	CloseNonModalDlg((CDialog*&)m_pSearchDlg);
	CloseNonModalDlg((CDialog*&)m_pChangeExitDlg);
	CloseNonModalDlg((CDialog*&)m_RegisterDlg); //关闭注册对放框(或QQ首次登录欢迎对话框)

	SAFE_DELETE_DIALOG(m_pTipCtrl) ;
}

void CHallDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_TREE_ROOMLIST, m_treeCtrl);
	DDX_Control(pDX, IDC_TICKER_BOTTOM, m_BottomTicker);
	CDialog::DoDataExchange(pDX);	
}

BEGIN_MESSAGE_MAP(CHallDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_SKIN_MESSAGE

	ON_COMMAND(ID_TRAYMENU_EXIT,			&CHallDlg::OnTraymenuExit)
	ON_COMMAND(ID_TRAYMENU_HIDEWND,			&CHallDlg::OnTraymenuHideWnd)
	ON_COMMAND(ID_TRAYMENU_USERSETTING,		&CHallDlg::OnTraymenuUserSet)
	ON_COMMAND(ID_TRAYMENU_VEDIOSETTING,	&CHallDlg::OnTraymenuVedioSet)
	ON_COMMAND(ID_TRAYMENU_HELP,			&CHallDlg::OnTraymenuHelp)
	ON_COMMAND(ID_TRAYMENU_ABOUT,			&CHallDlg::OnTraymenuAbout)
	ON_COMMAND(ID_TRAYMENU_CHANGEUSER,		&CHallDlg::OnTraymenuChangeUser)

	ON_MESSAGE(WM_MISSION_COMPLETED,			&CHallDlg::OnLoadHTMLComplete)	
	ON_MESSAGE(WM_MISSION_HYPERLINK_ENTERROOM,	&CHallDlg::OnEnterRoom)	
	ON_MESSAGE(WM_MISSION_HYPERLINK_OPENURL,	&CHallDlg::OnOpenURL)	
	ON_MESSAGE(WM_MISSION_HYPERLINK_INLINEURL,	&CHallDlg::OnInLineURL)
	ON_MESSAGE(WM_SKIN_PAINT,					&CHallDlg::OnPostPaint)
	ON_MESSAGE(WM_TIP_CLICK_CLOSE , &CHallDlg::OnTipClickClose )
	ON_WM_MOVE()
	ON_WM_SHOWWINDOW()
	ON_WM_NCMOUSEMOVE()
	ON_WM_MOVING()
	ON_MESSAGE(COMMONLIB_UM_NONDOMODAL_MSGBOX_GOTOREGISTER, &CHallDlg::OnClickMsgBoxGoToRegister)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

int CHallDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(m_pSkinEngine == NULL)
	{
		return -1;
	}
	else
	{
		HRESULT hr = m_pSkinEngine->LoadSkin(GetSafeHwnd(),L"HallDlg");
		ASSERT(hr== S_OK && __LINE__);
	}
	//最后一个参数为TRUE时则是上下方向的分割栏
	//if(!m_Splitter.Create(WS_CHILD | WS_VISIBLE, _T(""), CRect(0, 0, 0, 0), this, 1000, FALSE))
	//   return -1;

	//搜索框
	m_pSearchDlg = new CSearchDlg;
	if (! m_pSearchDlg->Create(CSearchDlg::IDD, this))
		return -1;

	NotifyCreateSkinItem(); 
	m_EnterRoomRequest.InitEnterRoom(this);

	CString strModulePath	= common::utility::systemhelper::Get179AppPath().c_str();	
	m_ResListiconPath.Format(_T("%s%s"),strModulePath,RESOUCRE_LISTCION);
	m_ResHeadPath.Format(_T("%s%s"),strModulePath,RESOUCRE_HEAD_PATH);


	return 0;
}

void CHallDlg::OnClose()
{
	if(m_pChangeExitDlg == NULL)
	{
		m_pChangeExitDlg = new CChangeExitDlg(m_pModuleManager,this);
		SwitchResourceToModule(_T("Hall"));
		m_pChangeExitDlg->Create(CChangeExitDlg::IDD, this);		
		m_pChangeExitDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pChangeExitDlg->ShowWindow(SW_SHOW);
	}

	EnableWindow(FALSE);
}

LRESULT CHallDlg::OnSkinMessage(WPARAM wParam,LPARAM lParam)
{
	switch(wParam)
	{
	case SKIN_ON_CLICK_MINIMIZE:
		{
			PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		}
		break;
	case SKIN_ON_CLICK_MAXIMIZE:
		{
			PostMessage(WM_SYSCOMMAND, IsZoomed() ? SC_RESTORE : SC_MAXIMIZE, 0);
		}
		break;
	case SKIN_ON_CLICK_CLOSE:
		{
			OnClose();
		}
		break;
		/*case SKIN_ON_SPLITTER_MOVE:
		{
		ModifySplitter((int)lParam);
		}
		break;*/
	case SKIN_ON_DIALOG_RESIZE:
		{
			DrawTreeView();
		}
		break;				
	case SKIN_ON_CLICK_HEAD:   //点击头像时的消息
		{
			OnClickHead();  //点击头像时的响应函数，以弹出个人资料等对话框
		}
		break;
	case SKIN_ON_CLICK_VIP:   //头像信息栏边的VIP图标点击消息
		{
			CString vipUrl,urlTemp;
			CSysSettingIni::GetWebInterfaceUrl(SYSSET_INI_KEYNAME_VIPURL,urlTemp);
			vipUrl = AppendUrlParamet(urlTemp);
			common::utility::systemhelper::OpenUrl(wstring(vipUrl));
		}
		break;		
	case SKIN_ON_CLICK_SYSSETTING: //头像信息栏边的设置按扭点击消息
		{				
			OnClickSysSetting(); //点击了头像信息栏边的设置按扭响应函数
		}
		break;
	case SKIN_ON_CLICK_MGRCENTER:  //头像信息栏边的管理中心图标点击消息
		{
			CString managerUrl,urlTemp;
			CSysSettingIni::GetWebInterfaceUrl(SYSSET_INI_KEYNAME_PERSONALINFOMANAGERCENTER,urlTemp);
			managerUrl = AppendUrlParamet(urlTemp);
			common::utility::systemhelper::OpenUrl(wstring(managerUrl));
		}
		break;
	case SKIN_ON_CLICK_LOBBY_HOME: //大厅导航栏上的大厅首页消息
		{
			InitAllRoomBrowser();//大厅右侧内嵌IE打开大厅首页的页面

		}
		break;
	case SKIN_ON_CLICK_ACCOUNT_PAY: //大厅导航栏上的账号冲值消息
		{
			CString payUrl,urlTemp;
			CSysSettingIni::GetWebInterfaceUrl(SYSSET_INT_KEYNAME_HALLPAY,urlTemp);
			payUrl = AppendUrlParamet(urlTemp);
			common::utility::systemhelper::OpenUrl(wstring(payUrl));  //使用浏览器打开账号冲值页面				

		}
		break;
	case SKIN_ON_CLICK_RANK: //大厅导航栏上的排行榜消息
		{
			CString rankUrl;
			CSysSettingIni::GetWebInterfaceUrl(SYSSET_INT_KEYNAME_HALLRANK,rankUrl); //大厅导航栏上的排行榜url
			SetAllRoomBrowser(wstring(rankUrl)); //大厅右侧内嵌IE打开排行榜的页面				
		}
		break;
	case SKIN_ON_CLICK_OFFICIAL_FORUM:  //大厅导航栏上的官方论坛消息
		{				
			CString forumUrl,urlTemp;
			CSysSettingIni::GetWebInterfaceUrl(SYSSET_INT_KEYNAME_HALLFORUM,urlTemp);
			forumUrl = AppendUrlParamet(urlTemp);
			common::utility::systemhelper::OpenUrl(wstring(forumUrl));  //使用浏览器打开官方论坛页面					

		}
		break;
	case SKIN_ON_CLICK_REFRESH:  //大厅导航栏上的刷新消息
		{
			Room_Create_And_Show Create_And_Show ;
			Create_And_Show.stEnterRoom.unRoomID = 1111 ;
			Create_And_Show.stEnterRoom.strRoomName = L"旋舞" ;
			Create_And_Show.stEnterRoom.relayIP_dianxin = 843425984 ;
			Create_And_Show.stEnterRoom.relayIP_wangtong = 843425984 ;
			Create_And_Show.stEnterRoom.relayPort_dianxin = 5179;
			Create_And_Show.stEnterRoom.relayPort_wangtong = 5179;
			Create_And_Show.stEnterRoom.strSignature = L"我是签名，lalalala";
			m_pModuleManager->CallService( Room::SERVICE_ROOM_CREATE_AND_SHOW ,(param)&Create_And_Show) ;
		}
		break;		
	case SKIN_ON_CLICK_HOME_PAGE:  //大厅导航栏上的179首页消息
		{
			CString s179Official;
			CSysSettingIni::GetWebInterfaceUrl(SYSSET_INI_KEYNAME_HALL179OFFICIAL,s179Official);
			s179Official = AppendUrlParamet(s179Official);
			common::utility::systemhelper::OpenUrl(wstring(s179Official));   //使用浏览器打开79官方首页				
		}
		break;		
	case SKIN_ON_CLICK_USER_HELP:  //大厅导航栏上的用户帮助消息
		{
			CString helpUrl;
			CSysSettingIni::GetWebInterfaceUrl(SYSSET_INI_KEYNAME_HALLHELP,helpUrl);
			helpUrl = AppendUrlParamet(helpUrl);
			common::utility::systemhelper::OpenUrl(wstring(helpUrl));    //使用浏览器打开用户帮助页面
		}
		break;
	case SKIN_ON_CLICK_BTN_BACK:  //房间搜索中返回按扭的消息
		{
			OnClickSearchBack();  //点击房间搜索中返回按扭时的响应函数
		}
		break;
	case SKIN_ON_CLICK_BTN_LOGIN:  //游客登录时头像右侧的"登录"按扭消息
		{
			OnClickBtnLogin(); //游客登录时头像右侧点击"登录"按扭时的响应函数
		}
		break;
	case SKIN_ON_CLICK_BTN_REGISTER:  //游客登录时头像右侧的"注册"按扭消息
		{
			OnClickBtnRegister();  //游客登录时头像右侧可点击"注册"按扭时的响应函数
		}
		break;
	case SKIN_ON_CLICK_BTN_SAFESETTING:
		{
			OnClickBtnSafeSetting() ;
		}
		break;
	case SKIN_ON_CLICK_SYS_FEEDBACK:
		{
			CString helpUrl;
			CSysSettingIni::GetWebInterfaceUrl(SYSSET_INI_KEYNAME_FEEDBACK,helpUrl);
			common::utility::systemhelper::OpenUrl(wstring(helpUrl));    //使用浏览器打开用户反馈页面				
		}
		break;
	default:
		break;
	}

	return TRUE;
}

BOOL CHallDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化	

	ModifyStyleEx(WS_EX_TOOLWINDOW,WS_EX_APPWINDOW);

	ModifyStyle( 0 , WS_CLIPCHILDREN ) ;

	//标题字体首选微软雅黑
	std::wstring strFont;
	common::utility::systemhelper::GetDefaultFontName(strFont);
	CComVariant var = (CComVariant) strFont.c_str();
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textTitle",L"fontFamily",var);

	ShowHeadUserInfo();

	//大厅列表TreeView初始化
	CString bkBit;
	m_treeCtrl.SetParentHWMD(this->m_hWnd);
	CString strTemp = common::utility::systemhelper::Get179AppPath().c_str();
	bkBit.Format(_T("%sresource\\halltree\\Hall_imgHallListTreeBk.png"),strTemp);
	m_treeCtrl.SetBkImgPath(bkBit);

	m_treeCtrl.SetItemHeight(32);

	m_halltree.Load();

	HallTreeSkinSet(false);
	SetRoomList();//默认显示大厅列表


	//创建显示对应所有房间浏览器
	m_htmlRight.CreateFromStatic(IDC_ALL_ROOM,this);
	var = (LONG)m_htmlRight.GetSafeHwnd();
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"itemHtmlAllRoom",L"hwnd",var);

	SetLoginToServer(false);  //设置 未登录到服务器


	CString curVerInfo = CAutoUpdate::GetCurVersionInfo();  //从配置文件中获取当前版本号
	var = curVerInfo;
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textVersion",L"text",var);  //设置客户端当前版本

	MoveTicker();	

	DrawGifButton();
	return TRUE;
}

void CHallDlg::OnTimer(UINT_PTR nIDEvent)
{	
	CDialog::OnTimer(nIDEvent);

	if(nIDEvent == PROCESS_REPORT_TIMER_IDEVENT )
	{
		m_processReport.OnProcessReport();
		return;
	}
	else if(nIDEvent == HALLTREE_TIMER_IDEVENT)
	{
		UpdateHallTree();//大厅目录树更新
		return;
	}	
	else if(nIDEvent == HALL_TICKER_TIMER_IDEVENT)
	{
		EventUpdateTicker(); //发送大厅底部跑马灯获取最新数据请求
		return;
	}

	//以下的定时器，若大厅最小化时不执行	
	if(IsIconic())
	{
		return;  
	}

	if ( nIDEvent == HALLSAFESETTING_TIMER_TRAY )
	{
		SafeSettingTimer() ;//(安全中心)密保
	}

}
void CHallDlg::ShowHeadUserInfo()
{
	core::CurInfo m_MyCurInfo = GetCurInfo();
	CComVariant var,strName,varImage,strAccount,strLevel,imgLevel,imgVIP,vipTip;	
	CString strtemp,svipTip, imgPath;	

	if(!m_MyCurInfo.bLogined)
	{
		//头像	
		imgPath = g_HallModule->GetLoaclHeadImage(0,0);
		varImage = g_HallModule->LoadImageWrapper(imgPath);
		m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"imgMyHead",L"externalImage",varImage);	

		//姓名、账号		
		strName = L"您好!  ";
		strAccount = L"(白兔KTV期待您的加入)";

	}
	else if(m_MyCurInfo.enmAccountType == core::ACCOUNTTYPE_VISITOR)  //游客，显示游客头像信息
	{
		//游客头像		
		imgPath = g_HallModule->GetLoaclHeadImage(0,0);
		varImage = g_HallModule->LoadImageWrapper(imgPath);
		m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"imgMyHead",L"externalImage",varImage);	

		//游客姓名、账号
		strName = m_MyCurInfo.strName.c_str();
		strtemp.Format(_T("(%d)"),m_MyCurInfo.un179id);
		strAccount = strtemp;		

		var = true;
		m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"btnRegister",L"visible",var);  //显示 "注册"按扭
		m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"btnLogin",L"visible",var); //显示 "登录"按扭

	}
	else  //用户登录成功，显示用户头像信息
	{		

		imgPath = g_HallModule->GetLoaclHeadImage(m_MyCurInfo.unUIN,m_MyCurInfo.bMan);
		varImage = g_HallModule->LoadImageWrapper(imgPath);
		m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"imgMyHead",L"externalImage",varImage);	

		//姓名、账号、等级		
		strName = GetCurNickName();
		strtemp.Format(_T("(%d)"),m_MyCurInfo.un179id);
		strAccount = strtemp;
		//strLevel = m_MyCurInfo.unUserLevel;		
		//GetImgLevelPath(m_MyCurInfo,strtemp);   //根据等级在线时长获取头像部分等级图标的路径
		//imgLevel = strtemp;
		GetVIPPath(m_MyCurInfo.enmVIPLevel,strtemp,svipTip);  //根据冠获取头像部分VIP图标的路径
		imgVIP = strtemp;
		vipTip = svipTip;
		//m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"imgLevel",L"image",imgLevel);
		//m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textLevel",L"text",strLevel);	
		m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"imgVIP",L"absIconImage",imgVIP);
		m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"imgVIP",L"tip",vipTip);

		SetRickLevelTip(); //设置富豪等级TIP提示语

		int totalTime = m_MyCurInfo.unOnlineTime + m_MyCurInfo.unLeftTime;  //等级进度条的总时长
		int finishTime = m_MyCurInfo.unOnlineTime;  //等级进度条的已完成时长
		//DrawLevelProgess(totalTime,finishTime);   //绘制等级进度条
	}   


	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textName",L"text",strName);
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textAccount",L"text",strAccount);	

	m_pSkinEngine->GetSubSkinProp(GetSafeHwnd(),L"textName",L"textWidth",&var);
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textName",L"width",var);

	m_pSkinEngine->GetSubSkinProp(GetSafeHwnd(),L"textAccount",L"textWidth",&var);
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textAccount",L"width",var);

	int iAccountWidth = var.intVal;
	CComVariant var1,var2;
	m_pSkinEngine->GetSubSkinProp(GetSafeHwnd(),L"textName",L"left",&var1);
	m_pSkinEngine->GetSubSkinProp(GetSafeHwnd(),L"textName",L"width",&var2);
	int iAccouttLeft = var1.intVal + var2.intVal;//这个是js里的代码一致，解决登录过程号码显示不全的问题
	var = iAccouttLeft;
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textAccount",L"left",var);


}

//头像更新
void CHallDlg::OnHeadImageUpdate(uint32 unUIN)
{
	core::CurInfo m_MyCurInfo = GetCurInfo();

	if(unUIN == m_MyCurInfo.unUIN)  //若更新的头像为当前用户头像，则在头像区域显示最新头像
	{
		CString imgPath;	
		imgPath = g_HallModule->GetLoaclHeadImage(m_MyCurInfo.unUIN,m_MyCurInfo.bMan);
		CComVariant varImage = g_HallModule->LoadImageWrapper(imgPath,true);
		m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"imgMyHead",L"externalImage",varImage);

	}


}

//头部信息初始化
void CHallDlg::HeadInit()
{
	SendMessage(WM_NOTIFY_HALL_IS_GUEST_LOGIN,0,0);

	CComVariant var;
	CComVariant strName;
	CComVariant strAccount;
	CString strTemp;

	//头像	
	strTemp = g_HallModule->GetLoaclHeadImage(0,0);
	CComVariant varImage = g_HallModule->LoadImageWrapper(strTemp);
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"imgMyHead",L"externalImage",varImage);

	//姓名、账号
	strName = L"您好!  ";
	strAccount = L"(白兔KTV期待您的加入)";

	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textName",L"text",strName);
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textAccount",L"text",strAccount);

	m_pSkinEngine->GetSubSkinProp(GetSafeHwnd(),L"textName",L"textWidth",&var);
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textName",L"width",var);

	m_pSkinEngine->GetSubSkinProp(GetSafeHwnd(),L"textAccount",L"textWidth",&var);
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textAccount",L"width",var);

	CRect rcTemp;
	m_pSkinEngine->GetSubSkinPos(GetSafeHwnd(),L"imgHallUserInfoBk",&(rcTemp.left),&(rcTemp.top),
		&(rcTemp.right),&(rcTemp.bottom));
	int iAccountWidth = var.intVal;
	CComVariant var1,var2;
	m_pSkinEngine->GetSubSkinProp(GetSafeHwnd(),L"textName",L"left",&var1);
	m_pSkinEngine->GetSubSkinProp(GetSafeHwnd(),L"textName",L"width",&var2);
	int iAccouttLeft = var1.intVal + var2.intVal;//这个是js里的代码一致，解决登录过程号码显示不全的问题
	var = iAccouttLeft;
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textAccount",L"left",var);

	var = false;
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"btnRegister",L"visible",var);  //不显示 "注册"按扭
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"btnLogin",L"visible",var);    //不显示 "登录"按扭
	
	if(m_htmlIsLoadComplete && IsHallHome())
	{
		_variant_t pResult;
		m_htmlRight.CallJScript(_T("closeVideo"),&pResult);
	}
	
	
	
}

//根据等级在线时长获取头像部分等级图标的路径
void CHallDlg::GetImgLevelPath(core::CurInfo &myCurInfo,CString &imglevel)
{

	if(myCurInfo.unUserLevel <= 19)   // 用户等级 >=0 且 用户等级<=19
	{
		imglevel = L"..\\user\\btnChangeUser.png";
	}
	else if(myCurInfo.unUserLevel <= 39)  // 用户等级 >=20 且 用户等级<=39
	{
		imglevel = L"..\\user\\Fish.png";
	}
	else if(myCurInfo.unUserLevel <= 59) // 用户等级 >=40 且 用户等级<=59
	{
		imglevel = L"..\\user\\Fish.png";
	}
	else if(myCurInfo.unUserLevel <= 79) // 用户等级 >=60 且 用户等级<=79
	{
		imglevel = L"..\\user\\Fish.png";
	}
	else if(myCurInfo.unUserLevel <= 99) // 用户等级 >=80 且 用户等级<=99
	{
		imglevel = L"..\\user\\Fish.png";
	}
	else  // 用户等级>=100
	{
		imglevel = L"..\\user\\Fish.png";
	}	
}

//根据冠获取头像部分VIP图标的路径
void CHallDlg::GetVIPPath(uint8 vipLevel,CString &imgVip,CString &vipTip)
{	
	switch(vipLevel)
	{
	case core::VIPLEVEL_NONE:  //未注册
		{
			imgVip.Format(_T("%sLevelVipNone.png"),m_ResListiconPath);	
			vipTip =_T("点击升级VIP");

		}
		break;
	case core::VIPLEVEL_REGISTER:  //已注册
		{
			imgVip.Format(_T("%sLevelVipNone.png"),m_ResListiconPath);		
			vipTip =_T("点击升级VIP");
		}
		break;
	case core::VIPLEVEL_VIP:   //vip
		{
			imgVip.Format(_T("%sLevelVip.png"),m_ResListiconPath);			
			vipTip =_T("普通VIP");
		}
		break;
	case core::VIPLEVEL_HONOR:  //荣耀vip
		{
			imgVip.Format(_T("%sLevelVipHonor.png"),m_ResListiconPath);		
			vipTip =_T("荣耀VIP");
		}
		break;
	case core::VIPLEVEL_SILVER:  //银冠
		{
			imgVip.Format(_T("%sLevelSilver.png"),m_ResListiconPath);
			vipTip.Format(_T("%s"),_T("银色皇冠"));
		}
		break;
	case core::VIPLEVEL_GOLDEN:  //金冠
		{
			imgVip.Format(_T("%sLevelGolden.png"),m_ResListiconPath);
			vipTip.Format(_T("%s"),_T("金色皇冠"));
		}
		break;
	case core::VIPLEVEL_CRYSTAL:  //水晶皇冠
		{
			imgVip.Format(_T("%sLevelCrystal.png"),m_ResListiconPath);
			vipTip.Format(_T("%s"),_T("水晶皇冠"));			
		}
		break;
	case core::VIPLEVEL_DIAMOND:  //钻石皇冠
		{
			imgVip.Format(_T("%slevelDiamond.png"),m_ResListiconPath);
			vipTip.Format(_T("%s"),_T("钻石皇冠"));			
		}
		break;
	case core::VIPLEVEL_BLACKDIAMOND:  //黑钻皇冠
		{
			imgVip.Format(_T("%sLevelBlackDiamond.png"),m_ResListiconPath);
			vipTip.Format(_T("%s"),_T("黑钻皇冠"));			
		}
		break;
	case core::VIPLEVEL_TOP:  //至尊皇冠
		{
			imgVip.Format(_T("%sLevelTop.png"),m_ResListiconPath);
			vipTip.Format(_T("%s"),_T("至尊皇冠"));					
		}
		break;

	default: 
		imgVip.Format(_T("%sLevelVipNone.png"),m_ResListiconPath);	
		break;
	}	
}

void CHallDlg::DrawLevelProgess(int nTotalTime,int nFinishTime) //绘制等级进度条
{
	core::CurInfo m_MyCurInfo = GetCurInfo();

	CRect rcTemp,rcLevelProgress;
	m_pSkinEngine->GetSubSkinPos(GetSafeHwnd(),L"progBack",&(rcTemp.left),&(rcTemp.top),&(rcTemp.right),&(rcTemp.bottom));	

	if(nTotalTime == 0)
		nTotalTime = 1;
	int nWidth = rcTemp.right - rcTemp.left;
	int nFullLength = (int)((( nFinishTime*100.0)/nTotalTime*nWidth)/100.0);
	int nWhiteLength = nWidth - nFullLength;

	CComVariant varBack,varFull,varWhite;
	varBack = L"empty.png";
	varFull = L"full.png";
	varWhite = L"white.png";

	CComVariant varFullLen,varWhiteLen,varLevel;
	CString tempLength;
	tempLength.Format(_T("%d"),nFullLength);
	varFullLen = (CComVariant) tempLength;
	tempLength.Format(_T("%d"),nWhiteLength);
	varWhiteLen = (CComVariant) tempLength;

	int ratio = nFinishTime*1.0/nTotalTime * 100;
	tempLength.Format(_T("%d%s"),ratio,L"%");
	varLevel = (CComVariant) tempLength;

	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"progBack",L"image",varBack);
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"progFull",L"image",varFull);
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"progWhite",L"image",varWhite);

	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"progFull",L"width",varFullLen);
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"progWhite",L"width",varWhiteLen);
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textProg",L"text",varLevel);
}

void CHallDlg::InitAllRoomBrowser()
{
	CString str179Url,tempUrl;
	CSysSettingIni::GetWebInterfaceUrl(SYSSET_INT_KEYNAME_HALLHOME,tempUrl);
	str179Url = AppendUrlParamet(tempUrl);

	m_htmlIsLoadComplete = false;
	phtmlRightResult = NULL;

	m_htmlRight.SetUrl(wstring(str179Url)); 
	m_htmlRight.UpdateURL();
	
}

//当前大厅右侧内嵌IE为大厅首页
bool CHallDlg::IsHallHome()
{
	CString str179Url,tempUrl;
	CSysSettingIni::GetWebInterfaceUrl(SYSSET_INT_KEYNAME_HALLHOME,tempUrl);
	str179Url = m_htmlRight.GetLocationURL();
	str179Url = str179Url.Left(str179Url.Find(_T("?")));
	if(str179Url.Right(1) == _T("/"))
	{
		str179Url = str179Url.Left(str179Url.GetLength()-1);
	}
	if(tempUrl.Right(1) == _T("/"))
	{
		tempUrl = tempUrl.Left(tempUrl.GetLength()-1);
	}
	if(str179Url == tempUrl)
	{
		return true;
	}
	else
	{
		return false;
	}

}

void CHallDlg::SetAllRoomBrowser(std::wstring url)
{
	m_htmlIsLoadComplete = false;
	m_htmlRight.SetUrl(url);
	m_htmlRight.UpdateURL();
}
void CHallDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	//CDialog::OnOK();
}

void CHallDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	//CDialog::OnCancel();
}


LRESULT CHallDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_LOADJSREADY:
		{
			//显示加载中...
			/*CComVariant var  = true;
			m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(), L"HTMLLoading2", L"visible", var);
			DISPPARAMS dispParam = {0, NULL, 0, 0};
			m_pSkinEngine->CallSubSkinMethod(GetSafeHwnd(), L"HTMLLoading2", L"Play", NULL, &dispParam);
			m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(), L"textWaitConnectting2", L"visible", var);

			CComVariant var2 = false;             
			m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(), L"textTryConnect2", L"visible", var2);
			m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(), L"btnTryConnect2", L"visible", var2);*/
		}
		break;
	case WM_NOTIFY_HALL_CHANGE_USER:
		{
			OnTraymenuChangeUser();
		}
		break;
	case WM_NOTIFY_HALL_EXIT:
		{
			OnTraymenuExit();
		}
		break;		
	case WM_NOTIFY_HALL_CLICK_SEARCH:
		{
			OnClickSearch();
		}
		break;
	case WM_TREECTRL_CLEAR:
		{
			m_treeCtrl.Clear();
		}
		break;
	case WM_NOTIFY_HALL_TREE_CLICK:  //点击目录树项的消息
		{					
			OnTreeClick((HTREEITEM)wParam);
		}
		break;
	case WM_NOTIFY_HALL_IS_GUEST_LOGIN:
		{
			NOTIFY_SKIN_HELPER(SKIN_NOTIFY_IS_GUEST,(LONG)0);
			MoveGifButton(FALSE);
		} 
		break;
	case WM_NOTIFY_HALL_NO_GUEST_LOGIN:
		{
			NOTIFY_SKIN_HELPER(SKIN_NOTIFY_NO_GUEST,(LONG)0);
			MoveGifButton(TRUE);
		}
		break;	
	case UM_QQFRISTLOGIN_INFO: //QQ首次登录欢迎对话框
		{
			CreateAndShowRegisterDlg(false,this);//QQ用户首次登录时的欢迎对话框
		}
		break;	
	case UM_USERINFO_UPDATE:  //用户资料变更
		{
			ShowHeadUserInfo(); //显示头部用户信息
		}
		break;
	case WM_MODULE_MSGBOX_SHOW:
		{
			Hall::MsgBox179 *pMsgbox = (Hall::MsgBox179*)(wParam);
			Module179MsgBoxShow(pMsgbox);
		}
		break;		
	case UM_RES_UPDATE_SUCCESS: //配置文件自动更新成功
		{
			m_pModuleManager->PushMessage(MakeMessage<MODULE_ID_HALL>()(Hall::MESSAGE_VALUE_HALL_RES_UPDATE,0 , 0, 0, 0));  //发送配置文件自动更新成功Message
		}
		break;

	default:
		break;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CHallDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);
	MoveSafeCenterTipCtrl();
	CComVariant var;
	m_pSkinEngine->GetSubSkinProp(GetSafeHwnd(), L"imgRichLevel", L"visible", &var);
	if (IsIconic())
	{
		var = FALSE;
	}

	MoveGifButton(var.boolVal);
}

void CHallDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	if ( bShow )
	{
		if ( m_pTipCtrl != NULL )
		{
			m_pTipCtrl->ShowWindow( SW_SHOW ) ;
		}
	}
	else
	{
		if ( m_pTipCtrl != NULL )
		{
			m_pTipCtrl->ShowWindow( SW_HIDE ) ;
		}
	}
	CComVariant var;
	m_pSkinEngine->GetSubSkinProp(GetSafeHwnd(), L"imgRichLevel", L"visible", &var);
	if (var.boolVal)
	{
		MoveGifButton(bShow);
	}else
	{
		MoveGifButton(FALSE);
	}
	//MoveGifButton(bShow);

}

void CHallDlg::NotifyCreateSkinItem()
{
	if (m_pSearchDlg->GetSafeHwnd() != NULL)
	{
		NOTIFY_SKIN_HELPER(SKIN_NOTIFY_SEARCH_DLG_HWND, m_pSearchDlg->GetSafeHwnd());
	}
}

//游客退出客户端时删除当前用户最近访问列表
void CHallDlg::DeleteVisitorsRoomList()
{
	core::CurInfo m_MyCurInfo = GetCurInfo();
	if(m_MyCurInfo.enmAccountType == core::ACCOUNTTYPE_VISITOR) //游客
	{
		CString sdelUrl,sTemp;
		CSysSettingIni::GetWebInterfaceUrl(SYSSET_INI_KEYNAME_DELETEVISITORSROOMLIST,sTemp);  //删除当前用户最近访问列表url (特指游客,在游客退出客户端时调用)
		sdelUrl = AppendUrlParamet(sTemp);

		webinterface::get_http_url_event_req getHttpEvent;
		getHttpEvent.srcMId	=	MODULE_ID_HALL;
		getHttpEvent.desMId	=	MODULE_ID_WEBSERVICE;
		getHttpEvent.wstrUrl = sdelUrl;		
		m_pModuleManager->CallService(getHttpEvent.eventValue,(param)&getHttpEvent);

	}
}


//切换用户之后的大厅初始化
void CHallDlg::ChangeUserInit()
{
	core::CurInfo m_MyCurInfo = GetCurInfo();
	if(m_MyCurInfo.enmAccountType == core::ACCOUNTTYPE_VISITOR) //游客
	{
		PostMessage(WM_NOTIFY_HALL_IS_GUEST_LOGIN,0,0);	
	}
	else //非游客
	{
		PostMessage(WM_NOTIFY_HALL_NO_GUEST_LOGIN,0,0);			
	}	
	ShowHeadUserInfo(); //显示头部用户信息

	//设置大厅状态栏名称
	CString  hallName;
	hallName.Format(_T("%s(大厅)"),GetCurNickName());
	SetWindowText(hallName);

	//初始化大厅目录树
	HallTreeSkinSet(false);
	UpdateHallTree();//大厅目录树更新
	m_pSearchDlg->ClearSearchKey();//清除搜索关键字
	SetTimer(HALLTREE_TIMER_IDEVENT,HALLTREE_TIMER_INTERVALTIME,NULL);//定时更新大厅目录树	
	SetTimer(HALL_TICKER_TIMER_IDEVENT,HALL_TICKER_TIMER_INTERVALTIME,NULL);  //大厅底部跑马灯定时获取最新数据
	SetTimer(PROCESS_REPORT_TIMER_IDEVENT,PROCESS_REPORT_TIMER_INTERVALTIME,NULL); //对手进程定时查找上传

	////初始化大厅右侧的内嵌IE	
	InitAllRoomBrowser();//大厅右侧内嵌IE打开大厅首页的页面

}
void CHallDlg::SetRoomList()
{
	m_treeCtrl.Clear();

	list<hallNode*> &hallRoom = m_halltree.m_HallNode;
	HTREEITEM hparent = NULL;

	m_treeCtrl.SetRoomView(hparent,hallRoom);  //设置大厅列表

	DrawTreeView();
}


void CHallDlg::DrawTreeView()
{
	if (m_bSearchResult)//搜索，并且有结果
	{
		CRect rcImgBtnBackBk(0,0,0,0);
		m_pSkinEngine->GetSubSkinPos(GetSafeHwnd(), L"imgBtnBackBk", &(rcImgBtnBackBk.left), &(rcImgBtnBackBk.top), &(rcImgBtnBackBk.right), &(rcImgBtnBackBk.bottom));
		CRect rcTreeCtrl(0,0,0,0);
		m_pSkinEngine->GetSubSkinPos(GetSafeHwnd(), L"itemHallTree", &(rcTreeCtrl.left), &(rcTreeCtrl.top), &(rcTreeCtrl.right), &(rcTreeCtrl.bottom));
		m_treeCtrl.MoveWindow(rcTreeCtrl.left, rcImgBtnBackBk.top + 30, rcTreeCtrl.Width(), rcTreeCtrl.Height() - 30);
	}else
	{
		//DragonZ:
		/*CRect rectBk,rectSearch,rect;
		m_pSkinEngine->GetSubSkinPos(GetSafeHwnd(),L"imageWhiteBackground",&(rectBk.left),&(rectBk.top),&(rectBk.right),&(rectBk.bottom));
		m_pSkinEngine->GetSubSkinPos(GetSafeHwnd(),L"imgSearchDlgBk",&(rectSearch.left),&(rectSearch.top),&(rectSearch.right),&(rectSearch.bottom));
		rect.left = rectBk.left;
		rect.top = rectSearch.bottom;
		rect.right = rectSearch.right;
		rect.bottom = rectBk.bottom;
		m_pSkinEngine->GetSubSkinPos(GetSafeHwnd(),L"tabBtnHallList",&(rectSearch.left),&(rectSearch.top),&(rectSearch.right),&(rectSearch.bottom));
		rect.top = rectSearch.bottom;
		m_treeCtrl.MoveWindow( rect.left, rect.top, rect.Width(), rect.Height());*/

		CRect rcUserInfoBk(0,0,0,0), rcBottomLeftBk(0,0,0,0), rcSearchDlg(0,0,0,0);
		m_pSkinEngine->GetSubSkinPos(GetSafeHwnd(),L"imgHallUserInfoBk",&(rcUserInfoBk.left),&(rcUserInfoBk.top),&(rcUserInfoBk.right),&(rcUserInfoBk.bottom));
		m_pSkinEngine->GetSubSkinPos(GetSafeHwnd(),L"imgHallBottomLeftBk",&(rcBottomLeftBk.left),&(rcBottomLeftBk.top),&(rcBottomLeftBk.right),&(rcBottomLeftBk.bottom));
		m_pSkinEngine->GetSubSkinPos(GetSafeHwnd(),L"itemSearchDlg",&(rcSearchDlg.left),&(rcSearchDlg.top),&(rcSearchDlg.right),&(rcSearchDlg.bottom));
		m_treeCtrl.MoveWindow(rcUserInfoBk.left + 8, rcSearchDlg.bottom + 10, rcUserInfoBk.right - 5 - rcUserInfoBk.left - 8, rcBottomLeftBk.top - rcSearchDlg.bottom - 10);
	}

}

void CHallDlg::OnTraymenuHideWnd()
{
	if (this->IsWindowVisible())
	{
		this->ShowWindow(SW_HIDE);
	}
	else
	{
		this->ShowWindow(SW_SHOW);
		if ( m_pTipCtrl != NULL && m_pTipCtrl->isHided() )
		{
			m_pTipCtrl->ShowWindow( SW_HIDE ) ;
		}
	}    
}

void CHallDlg::OnTraymenuUserSet()
{	
	ShowSysSettingDlg(Show_User_Info_Dlg);
}

void CHallDlg::OnTraymenuVedioSet()
{	
	ShowSysSettingDlg(Show_Audio_Setting_Dlg);
}

void CHallDlg::OnTraymenuHelp()
{
	CString helpUrl;
	CSysSettingIni::GetWebInterfaceUrl(SYSSET_INI_KEYNAME_HALLHELP,helpUrl);
	helpUrl = AppendUrlParamet(helpUrl);
	common::utility::systemhelper::OpenUrl(wstring(helpUrl));    //使用浏览器打开用户帮助页面

}

void CHallDlg::OnTraymenuAbout()
{
	SwitchResourceToModule(_T("Hall"));
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CHallDlg::OnTraymenuChangeUser()
{
	if(m_pChangeExitDlg)
	{
		m_pChangeExitDlg->ShowWindow(SW_HIDE);
	}

	g_HallModule->ShowLoginDialog(true);
}

void CHallDlg::OnTraymenuExit()
{
	DeleteVisitorsRoomList();	

	this->ShowWindow(SW_HIDE);
	m_halltree.Unload();   //释放目录树
	if(NULL != m_pModuleManager)
		m_pModuleManager->PushEvent(MakeEvent<MODULE_ID_CORE>()(EVENT_VALUE_SYS_MAIN_LOOP_EXIT,MODULE_ID_HALL));
}


//点击了头像信息栏边的设置按扭响应函数
void CHallDlg::OnClickSysSetting()
{	
	ShowSysSettingDlg(Show_User_Info_Dlg);
}
//点击头像时的响应函数，以弹出个人资料等对话框
void CHallDlg::OnClickHead()
{
	Hall::HeadSetting * pHeadEvent = new Hall::HeadSetting();
	pHeadEvent->m_parentHwnd = m_hWnd;

	Event _event = MakeEvent<MODULE_ID_HALL>()(Hall::EVENT_VALUE_HALL_HEAD_UPLOAD, 
		MODULE_ID_HALL,0,0,0,pHeadEvent);

	g_HallModule->ProcessEvent(_event);	
	
}

//显示个人设置对话框
void CHallDlg::ShowSysSettingDlg(SysSettingDlgType dlgType)
{	
	SysSettingType sysSetting;
	sysSetting.dlgType = dlgType;
	sysSetting.pParentWnd = this->m_hWnd;

	m_pModuleManager->CallService(Room::SERVICE_ROOM_SHOW_SYSSETTING, (param)&sysSetting);
}


//大厅右侧内嵌页面中接口发送的浏览器打开新页面(带tocken参数)请求
LRESULT CHallDlg::OnOpenURL(WPARAM wParam,LPARAM lParam)
{
	LPCTSTR lpszURL = (LPCTSTR)wParam;
	std::wstring strURL = lpszURL;
	g_HallModule->Invoke(Hall::SERVICE_VALUE_HALL_OPEN_TOKEN_URL, (param)&strURL);	
	return S_OK;
}

//大厅右侧内嵌页面中接口发送的内嵌页面打开新页面(带tocken参数)请求
LRESULT CHallDlg::OnInLineURL(WPARAM wParam,LPARAM lParam)
{
	CString strUrl;	
	CString strParam = (LPCTSTR)wParam;
	strUrl = AppendUrlParamet(strParam);
	SetAllRoomBrowser(std::wstring(strUrl));

	return S_OK;
}


//HTML会抢焦点，所以加载完毕时，强制LoginDialog获取焦点
LRESULT CHallDlg::OnLoadHTMLComplete(WPARAM wParam,LPARAM lParam)
{

	CWnd *pwnd = GetFocus();
	if(pwnd->GetSafeHwnd() != g_HallModule->GetLoginDlgWnd()->GetSafeHwnd() && g_HallModule->GetLoginDlgWnd()->IsWindowVisible())
	{
		::SendMessage(g_HallModule->GetLoginDlgWnd()->GetSafeHwnd(),UM_LOGIN_DLG_SETFOCUS,0,0);
	}		
	
	m_htmlIsLoadComplete = true;
	core::CurInfo m_MyCurInfo = GetCurInfo();
	if(m_MyCurInfo.bLogined && IsHallHome())
	{
		if(!phtmlRightResult.boolVal)
		{
			_variant_t pResult;
			m_htmlRight.CallJScript(_T("openVideo"),&phtmlRightResult);	
			phtmlRightResult.boolVal = true;

		}
		
	}	

	return S_OK;
}


//大厅右侧内嵌页面中接口发送的进房请求
LRESULT CHallDlg::OnEnterRoom(WPARAM wParam, LPARAM lParam)
{
	enterRoomInfo enRoom;
	enRoom.m_RoomID = wParam;
	enRoom.m_hideLogin = lParam != 0;
	m_EnterRoomRequest.EnterRoomEventRequest(enRoom);  //请申进房
	return S_OK;
}


CSearchDlg* CHallDlg::GetSearchDlg()
{
	return m_pSearchDlg;
}


//点击目录树项的响应函数
void CHallDlg::OnTreeClick(HTREEITEM pItem)
{
	hallNode* pNode = (hallNode*)m_treeCtrl.GetItemData(pItem);
	assert(pNode != NULL);

	if(pNode == NULL)
		return;

	if(m_showType == TREE_RESULTINFO)  //房间搜索结果目录树
	{
		enterRoomInfo enRoom;
		enRoom.m_RoomID = pNode->m_RoomID;
		m_EnterRoomRequest.EnterRoomEventRequest(enRoom);  //请申进房
	} 
	else //大厅目录树 
	{
		if(!pNode->m_Url.IsEmpty()) 
		{
			CString hallUrl;		
			if(pNode->m_Url.Right(1) == _T("?"))  //url的最后一个字符是否为?  若是则带参，否则不带参
			{
				hallUrl = AppendUrlParamet(pNode->m_Url.Left(pNode->m_Url.GetLength() -1));
			}
			else
			{
				hallUrl = pNode->m_Url;
			}

			//白兔视频交友 url 特殊处理 使用浏览器打开
			CString strBaiTuUrl,strHallUrl;
			CSysSettingIni::GetWebInterfaceUrl(SYSSET_INI_KEYNAME_BAITUURL,strBaiTuUrl);  //获取白兔视频交友 url
			strBaiTuUrl.MakeLower();
			strHallUrl = hallUrl;
			strHallUrl.MakeLower();

			if(strHallUrl.Find(strBaiTuUrl) != -1 && !strBaiTuUrl.IsEmpty())
			{
				common::utility::systemhelper::OpenUrl(hallUrl.GetBuffer(0));	//使用浏览器打开对应url
				return;
			}
			//白兔视频交友 url 特殊处理 使用浏览器打开

			CString temp = hallUrl;
			temp.MakeLower();

			if(temp.Find(_T("newwnd=true")) != -1)
			{
				CreateAndShowGameDlg(this);
				m_GameDlg->SetURL(hallUrl);
				return;
			}

			if (pNode->m_NewUrl)
			{
				common::utility::systemhelper::OpenUrl(hallUrl.GetBuffer(0));	//使用浏览器打开对应url
			}
			else
			{
				SetAllRoomBrowser(wstring(hallUrl));  //大厅右侧内嵌IE打开指定的页面
			}
		}
	}
}

void CHallDlg::OnClickSearch()
{
	if(m_pSearchDlg->m_SearchNode.size() > 0) //房间搜索有结果
	{
		HallTreeSkinSet(true,true);
		SetResultList();
	}
	else //房间搜索无结果
	{
		HallTreeSkinSet(true,false);
	}

}

//点击房间搜索中返回按扭的响应函数
void CHallDlg::OnClickSearchBack()
{
	HallTreeSkinSet(false);
	SetRoomList();

}


//大厅目录树皮肤设置   
//isSearch 为true时设置目录树为搜索样式, 为false时设置目录树为大厅/我的房间样式
//searchFind 只当isSearch为true时有用， 搜索是否成功
void CHallDlg::HallTreeSkinSet(bool isSearch,bool searchFind)
{
	CComVariant isSearchVar,noSearchFindVar,isHallMyRoomVar;
	if(isSearch)  //设置目录树为搜索样式
	{
		isSearchVar = TRUE;
		isHallMyRoomVar = FALSE;
		if(searchFind) //搜索是否成功
		{
			noSearchFindVar = FALSE;
			m_treeCtrl.ShowWindow(SW_SHOW);
		}
		else
		{
			noSearchFindVar = TRUE;
			m_treeCtrl.ShowWindow(SW_HIDE);
		}

		m_showType = TREE_RESULTINFO;

		if(m_pSearchDlg->IsSearchKeyOK()) //搜索关键字正确
		{
			CComVariant textNoFound = SEARCH_NOFIND_KEYTRUE;
			m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textNoFound",L"text",textNoFound);
		}
		else //搜索关键字不正确
		{
			CComVariant textNoFound = SEARCE_NOFIND_KEYFALSE;
			m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textNoFound",L"text",textNoFound);
		}

	}
	else  //设置目录树为大厅/我的房间样式
	{
		isSearchVar = FALSE;
		noSearchFindVar = FALSE;
		isHallMyRoomVar = TRUE;
		m_treeCtrl.ShowWindow(SW_SHOW);

		m_showType = TREE_HALLINFO;

	}


	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"imgBtnBackBk",L"visible",isSearchVar);
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"btnBack",L"visible",isSearchVar);
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textSearch",L"visible",isSearchVar);
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textNoFound",L"visible",noSearchFindVar);

	//m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"imgTabRoundBackgroundInner",L"visible",isHallMyRoomVar);
	//DragonZ:
	//m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"tabBtnHallList",L"visible",isHallMyRoomVar);

	if ((isSearchVar.boolVal == TRUE) && (noSearchFindVar.boolVal == FALSE))//说明查找有结果
	{
		m_bSearchResult = TRUE;
	}
	else
	{
		m_bSearchResult = FALSE;
	}
}

void CHallDlg::SetResultList()
{	
	m_treeCtrl.Clear();

	list<hallNode*> &searchNode = m_pSearchDlg->m_SearchNode;
	HTREEITEM hparent = NULL;

	m_treeCtrl.SetRoomView(hparent,searchNode);  //设置搜索结果列表

	DrawTreeView();
}

void CHallDlg::OnClickBtnLogin()
{
	g_HallModule->ShowLoginDialog(true);
}

void CHallDlg::OnClickBtnRegister()
{
	CreateAndShowRegisterDlg(true,this);//创建注册对话框    
}

//安全中心
void CHallDlg::OnClickBtnSafeSetting() 
{
	core::CurInfo myCurInfo = GetCurInfo();
	CString SafeSettingUrl,urlTemp;
	if ( myCurInfo.enmAccountType == core::ACCOUNTTYPE_QQ )
	{
		//使用QQ帐户登陆的用户
		CSysSettingIni::GetWebInterfaceUrl(SYSSET_INI_KEYNAME_QQSAFESETTING,SafeSettingUrl);
		common::utility::systemhelper::OpenUrl(wstring(SafeSettingUrl));  //使用浏览器打开QQ安全中心页面	
	}
	else
	{

		CSysSettingIni::GetWebInterfaceUrl(SYSSET_INI_KEYNAME_SAFESETTING,urlTemp);
		SafeSettingUrl = AppendUrlParamet(urlTemp);
		common::utility::systemhelper::OpenUrl(wstring(SafeSettingUrl));  //使用浏览器打开安全中心页面	
	}
}

//大厅目录树文件更新
void CHallDlg::UpdateHallTree()
{
	CString strModulePath	= common::utility::systemhelper::Get179AppPath().c_str();	

	CString hallTreeUrl,hallTreeLocal;
	CString moduleParentPath;
	CSysSettingIni::GetloginUpdateURL(SYSSET_INT_KEYNAME_HALLTREEFILE,hallTreeUrl);  //获取登录成功之后要下载的HallTree.xml配置文件的url

	hallTreeLocal.Format(_T("%s%s"),strModulePath,XML_HALLTREE_FILE);

	if(hallTreeUrl.IsEmpty() || hallTreeLocal.IsEmpty())
	{
		return ; 
	}

	webinterface::get_download_file_event_req getDownloadEventReq;
	getDownloadEventReq.srcMId	=	MODULE_ID_HALL;
	getDownloadEventReq.desMId	=	MODULE_ID_WEBSERVICE;
	getDownloadEventReq.wstrUrl = hallTreeUrl;
	getDownloadEventReq.wstrFileName = hallTreeLocal;		
	m_nHallTeeSeqNo = m_pModuleManager->CallService(getDownloadEventReq.eventValue,(param)&getDownloadEventReq);   

}

//大厅目录树文件更新返回响应函数
void CHallDlg::OnUpdateHallTreeResp(Event const& evt)
{	
	uint32 nSeqNo = evt.param2;
	if(m_nHallTeeSeqNo != nSeqNo)
	{
		return;
	}

	if( evt.param0 == webinterface::WEB_RET_SUCCESS)
	{
		webinterface::get_download_file_event_resp* pDownEvent =(webinterface::get_download_file_event_resp* ) (evt.m_pstExtraInfo);
		m_halltree.Load();

		if(m_showType == TREE_HALLINFO)
		{			
			SetRoomList();
		}		
	}
}

//大厅底部跑马灯获取最新数据返回响应函数
void CHallDlg::OnUpdateTickerResp(Event const& evt)
{
	if(m_TickerSeqNo == (uint32)evt.param2)
	{
		if( evt.param0 == webinterface::WEB_RET_SUCCESS)
		{
			ASSERT(evt.m_pstExtraInfo != NULL);
			webinterface::get_http_url_event_resp* phttpResp =(webinterface::get_http_url_event_resp* ) (evt.m_pstExtraInfo);
			ParserUpdateTicker(phttpResp->strBuffer);
		}

	}
}


//是否已登录服务器
void CHallDlg::SetLoginToServer(bool isLogin)
{
	CComVariant varImage,varText;
	CString appPath = common::utility::systemhelper::Get179AppPath().c_str();
	CString strImgLoginToServer;

	if(isLogin)
	{
		strImgLoginToServer .Format(L"%sresource\\halltree\\Hall_imgLoginToServer_Success.png", appPath);		
		varText = L"已登录服务器";
	}
	else
	{
		strImgLoginToServer .Format(L"%sresource\\halltree\\Hall_imgLoginToServer_Fail.png", appPath);
		varText = L"未登录服务器";
	}
	varImage = strImgLoginToServer;	

	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(), L"imgLoginToServer", L"absoluteImage", varImage);	
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"textLoginToServer",L"text",varText);		

}

//QQ首次登录欢迎对话框是否正在显示
bool CHallDlg::QQDlgIsShow()
{
	bool qqDlg = false;
	if(m_RegisterDlg != NULL && m_RegisterDlg->IsWindowVisible())
	{
		if(m_RegisterDlg->GetDlgType() == Dlg_Type_For_CompleteInfo)
		{
			qqDlg = true;
		}
	}

	return qqDlg;
}
//(安全中心)密保
void CHallDlg::SafeSettingTimer()
{
	if ( m_bGraySafeSetting )
	{
		CComVariant varIconImage = L"Hall_iconSafeCenterGray.png";  //灰色
		m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"btnSafeSetting",L"iconImage",varIconImage);
	}
	else
	{
		CComVariant varIconImage = L"Hall_iconSafeCenterGold.png";  //金色
		m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"btnSafeSetting",L"iconImage",varIconImage);
	}
	m_bGraySafeSetting = !m_bGraySafeSetting ;
}

//关闭所有对话框
void CHallDlg::CloseAllDialog()
{	
	if(m_pChangeExitDlg && m_pChangeExitDlg->IsWindowVisible())
	{
		m_pChangeExitDlg->SendMessage(WM_CLOSE,0,0);
	}
	CloseRegisterDlg();
	CloseHallGameDlg();
}

void CHallDlg::CreateAndShowGameDlg(CWnd *pParent)
{
	if(m_GameDlg != NULL)
	{
		if(m_GameDlg->IsWindowVisible())
		{
			m_GameDlg->SetActiveWindow() ;
			return;
		}
		CloseHallGameDlg();
	}

	if(m_GameDlg == NULL)
	{
		SwitchResourceToModule(_T("Hall"));
		
		m_GameDlg = new CGameDlg(this->m_hWnd, pParent);

		if(m_GameDlg)
		{
			m_GameDlg->Create(CRegisterDlg::IDD,pParent);
			m_GameDlg->ShowWindow(SW_SHOW);
			m_GameDlg->CenterWindow(pParent);
		}
	}
}


//创建注册对话框(或QQ用户首次登录时的完善资料对话框)
//bRegister 为true时创建注册对话框， 为false时创建QQ用户首次登录完善资料对话框
void CHallDlg::CreateAndShowRegisterDlg(bool bRegister,CWnd *pParent)
{	
	if(m_RegisterDlg != NULL)
	{
		if(m_RegisterDlg->RoomRegisterDlgIsShow() && bRegister) //当前是否正在显示房间注册框
		{
			//当前正在显示房间注册框时，  再从登录框与游客的右上角点击"注册"按扭时，不再弹出新的注册框
			m_RegisterDlg->SetActiveWindow() ;
			return;
		}
		CloseRegisterDlg();
	}
	if(m_RegisterDlg == NULL)
	{
		SwitchResourceToModule(_T("Hall"));
		if(bRegister) //注册对话框
		{			
			m_RegisterDlg = new CRegisterDlg(g_HallModule->GetLoginDlgWnd()->GetSafeHwnd(),Dlg_Type_For_Register,pParent);		
		}
		else //QQ用户首次登录时的完善资料对话框
		{			
			m_RegisterDlg = new CRegisterDlg(this->m_hWnd,Dlg_Type_For_CompleteInfo,pParent);		
		}

		if(m_RegisterDlg)
		{
			m_RegisterDlg->Create(CRegisterDlg::IDD,pParent);
			EnableWindow(FALSE);
			m_RegisterDlg->ShowWindow(SW_SHOW);
			m_RegisterDlg->SetRoomRegister(false);
			m_RegisterDlg->CenterWindow(pParent);
		}
	}
}


//房间注册框是否正在显示
bool CHallDlg::RoomRegisterDlgIsShow()
{
	if(m_RegisterDlg != NULL)
	{
		return m_RegisterDlg->RoomRegisterDlgIsShow();
	}

	return false;

}

//设置了(安全中心)密保
void CHallDlg::OnPassWordSafe() 
{
	CComVariant varPassWordSafe = L"Hall_iconSafeCenterNormal.png";  
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"btnSafeSetting",L"iconImage",varPassWordSafe);
	if ( m_pTipCtrl != NULL )
	{
		m_pTipCtrl->ShowWindow( SW_HIDE ) ;
	}
	KillTimer( HALLSAFESETTING_TIMER_TRAY ) ;
}

void CHallDlg::OnLoginSuccess() 
{
	if ( m_pTipCtrl != NULL )
	{
		m_pTipCtrl->ShowWindow( SW_HIDE ) ;
	}
	KillTimer( HALLSAFESETTING_TIMER_TRAY ) ;
	core::CurInfo myCurInfo = GetCurInfo();
	if ( myCurInfo.enmAccountType == core::ACCOUNTTYPE_QQ )
	{
		//使用QQ帐户登陆的用户
		CComVariant var = L"Hall_iconSafeCenterQQNormal.png";  //QQ安全中心图标
		m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"btnSafeSetting",L"iconImage",var);
		if ( m_pTipCtrl != NULL )
		{
			m_pTipCtrl->ShowWindow( SW_HIDE ) ;
		}
		var = L"QQ安全中心" ;
		m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"btnSafeSetting",L"tip",var);
	}
	else
	{
		CComVariant var = L"密保邮箱" ;
		m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"btnSafeSetting",L"tip",var);

		if ( myCurInfo.enmVIPLevel == core::VIPLEVEL_NONE )
		{
			return ;
		}
		if ( myCurInfo.bPasswordSafe )
		{
			//设置过密保
			CComVariant varIconImage = L"Hall_iconSafeCenterNormal.png";  
			m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"btnSafeSetting",L"iconImage",varIconImage);
		}
		else
		{

			if ( myCurInfo.bRmbUser || myCurInfo.enmVIPLevel >= core::VIPLEVEL_VIP )
			{
				//冲过值 或者大于等于VIP
				SetTimer( HALLSAFESETTING_TIMER_TRAY , HALLSAFESETTING_TIMER_INTERVALTIME , NULL ) ;

				CString strSafeCenterTip ;
				CConfigManager::Instance()->GetUserInfo_Setting( GetCurInfo().unUIN , USERINFO_PERSONALSETTING_SAFECENTERTIP , strSafeCenterTip );
				if ( strSafeCenterTip.IsEmpty() || strSafeCenterTip == L"0" )
				{
					int nWidth = 0 ;
					int nHeight = 0 ;
					CRect RectSafeSettig ;
					m_pSkinEngine->GetSubSkinPos(GetSafeHwnd(),L"btnSafeSetting",&(RectSafeSettig.left),&(RectSafeSettig.top),
						&(RectSafeSettig.right),&(RectSafeSettig.bottom));
					ClientToScreen( &RectSafeSettig ) ;
					CRect RectHeadCtrl ;
					m_pSkinEngine->GetSubSkinPos(GetSafeHwnd(),L"imgMyHeadCtrlBack_N",&(RectHeadCtrl.left),&(RectHeadCtrl.top),
						&(RectHeadCtrl.right),&(RectHeadCtrl.bottom));
					ClientToScreen( &RectHeadCtrl ) ;
					if ( m_pTipCtrl == NULL )
					{
						m_pTipCtrl = new C179TipCtrl(eArrowPos::ARROW_BOTTOM_MIDDLE) ;
						m_pTipCtrl->CreateEx(this,0);
						m_pTipCtrl->SetTipText(L"为了您的账号安全，请申请密码保护");
						m_pTipCtrl->SetClose(TRUE);
					}
					m_pTipCtrl->ShowTip( RectSafeSettig.left+RectSafeSettig.Width()/2- (RectHeadCtrl.right+5), nWidth , nHeight );
					m_pTipCtrl->MoveWindow( RectHeadCtrl.right+40 , RectHeadCtrl.top+5 , nWidth ,nHeight ) ;	
					m_pTipCtrl->ShowWindow( SW_SHOW ) ;
				}
			}
			CComVariant varIconImage = L"Hall_iconSafeCenterGray.png";  //灰色
			m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(),L"btnSafeSetting",L"iconImage",varIconImage);
		}
	}

}

void CHallDlg::OnLoginFailed() 
{

	if ( m_pTipCtrl != NULL )
	{
		m_pTipCtrl->ShowWindow( SW_HIDE ) ;
	}
	KillTimer( HALLSAFESETTING_TIMER_TRAY ) ;

}

//关闭注册对话框(或QQ首次登录欢迎对话框)
void CHallDlg::CloseRegisterDlg()
{
	if(m_RegisterDlg != NULL)
	{
		if(RoomRegisterDlgIsShow())
		{
			g_HallModule->m_pModuleManager->CallService(Room::SERVICE_ROOM_CLOSED_REGISTER_DLG, NULL);
		}		

		if(!g_HallModule->GetLoginDlgWnd()->IsWindowVisible()) //登录框不显示时，为从大厅游客头像信息区点击的“注册”按扭，现需使用大厅可操作
		{
			EnableWindow(TRUE);
		}
		else //登录框显示时，为从登录框中点击的“注册”按扭，现需使登录框可操作
		{
			g_HallModule->GetLoginDlgWnd()->EnableWindow(TRUE);
		}
		CloseNonModalDlg((CDialog*&)m_RegisterDlg);

	}
}

void CHallDlg::CloseHallGameDlg()
{
	if(m_GameDlg != NULL)
	{
		CloseNonModalDlg((CDialog*&)m_GameDlg);
	}
}


int32 CHallDlg::Module179MsgBoxShow(Hall::MsgBox179 *pMsgbox)
{
	if(pMsgbox != NULL)
	{
		if(pMsgbox->m_boxStype == Hall::MsgBox179::MSGBOX_OK_ONE_STRING || pMsgbox->m_boxStype == Hall::MsgBox179::MSGBOX_OK_TWO_STRING ||
			pMsgbox->m_boxStype == Hall::MsgBox179::MSGBOX_OK_ONE_STRING_WITH_LINK || pMsgbox->m_boxStype == Hall::MsgBox179::MSGBOX_OK_TWO_STRING_WITH_LINK ||
			pMsgbox->m_boxStype == Hall::MsgBox179::MSGBOX_GUEST_OPERATE || pMsgbox->m_boxStype == Hall::MsgBox179::MSGBOX_OK_THREE_STRING ||
			pMsgbox->m_boxStype == Hall::MsgBox179::MSGBOX_OK_THREE_STRING_WITH_LINK)
		{
			return C179MsgBox::Show(this,pMsgbox->m_boxStype,pMsgbox->m_sMessage1.c_str(),pMsgbox->m_sMessage2.c_str(),pMsgbox->m_sMessage3.c_str(),pMsgbox->m_linkText.c_str(),pMsgbox->m_linkLocal.c_str(),pMsgbox->m_sTitle.c_str());
		}
		if(pMsgbox->m_boxStype == Hall::MsgBox179::MSGBOX_ICO_OK_ONE_STRING || pMsgbox->m_boxStype == Hall::MsgBox179::MSGBOX_ICO_OK_TWO_STRING ||
			pMsgbox->m_boxStype == Hall::MsgBox179::MSGBOX_ICO_OK_CANCEL_ONE_STRING || pMsgbox->m_boxStype == Hall::MsgBox179::MSGBOX_ICO_OK_CANCEL_TWO_STRING)
		{
			return C179MsgBox::Show(this,pMsgbox->m_boxStype,pMsgbox->m_boxICO,pMsgbox->m_sMessage1.c_str(),pMsgbox->m_sMessage2.c_str(),pMsgbox->m_sTitle.c_str());
		}	

	}

	return -1;

}


void CHallDlg::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	

	CDialog::OnNcMouseMove(nHitTest, point);
}


void CHallDlg::OnSize(UINT nType, int cx, int cy) 
{
	if(IsWindowVisible())
	{
		MoveTicker();
	}

	CDialog::OnSize(nType, cx,cy);
}

LRESULT CHallDlg::OnPostPaint(WPARAM wParam, LPARAM lParam)
{
	if(wParam == 0)
		return S_FALSE;

	HDC hDC = (HDC)wParam;
	m_BottomTicker.OnPostParentPaint(hDC);

	return S_OK;
}

LRESULT CHallDlg::OnTipClickClose(WPARAM wParam, LPARAM lParam)
{
	CConfigManager::Instance()->SetUserInfo_Setting( GetCurInfo().unUIN , USERINFO_PERSONALSETTING_SAFECENTERTIP , L"1") ;
	return S_OK ;
}

void CHallDlg::MoveTicker()
{
	CComVariant var1,var2,var3,var4;
	m_pSkinEngine->GetSubSkinProp(GetSafeHwnd(),L"imgLoginToServer",L"left",&var1);
	m_pSkinEngine->GetSubSkinProp(GetSafeHwnd(),L"imgLoginToServer",L"width",&var2);
	//DragonZ:
	m_pSkinEngine->GetSubSkinProp(GetSafeHwnd(),L"imgHallBottomLeftBk",L"top",&var3);
	m_pSkinEngine->GetSubSkinProp(GetSafeHwnd(),L"imgHallBottomLeftBk",L"height",&var4);
	int iTickerLeft = var1.intVal + var2.intVal + 220;
	int iTickerTop = var3.intVal + 1;
	int iTickerHeight = var4.intVal - 2 ;
	m_BottomTicker.MoveWindow(iTickerLeft,iTickerTop,0,iTickerHeight);
}

//发送大厅底部跑马灯获取最新数据请求
void CHallDlg::EventUpdateTicker()
{
	CString sTickerUrl,sTemp;
	CSysSettingIni::GetWebInterfaceUrl(SYSSET_INI_KEYNAME_HALLBOTTOMTICKER,sTickerUrl);  //大厅底部跑马灯获取最新数据url	

	webinterface::get_http_url_event_req getHttpEvent;
	getHttpEvent.srcMId	=	MODULE_ID_HALL;
	getHttpEvent.desMId	=	MODULE_ID_WEBSERVICE;
	getHttpEvent.wstrUrl = sTickerUrl;
	getHttpEvent.nTimeout = 3;//3秒
	getHttpEvent.bAllowOffline = true;
	IModuleManager* pModuleManager = g_HallModule->m_pModuleManager;
	if (pModuleManager != NULL)
	{
		m_TickerSeqNo = pModuleManager->CallService(getHttpEvent.eventValue,(param)&getHttpEvent);		
	}
}

//解释大厅底部跑马灯获取的最新数据
void CHallDlg::ParserUpdateTicker(std::string tickerResp)
{
	try
	{
		Json::Reader reader;
		Json::Value root;
		if(!reader.parse(tickerResp,root,false))
		{
			return ;
		}
		if(!root.isArray())
		{
			return;
		}

		vector<common::ui::tickerInfo> m_tickerVec;   
		std::string tickerMessage,tickerUrl;		
		int rootsize = root.size();
		for(int i=0;i<rootsize;++i)
		{
			tickerMessage = root[i][TIKCER_RESULTS_CONTENT].asString();
			tickerUrl = root[i][TIKCER_RESULTS_URL].asString();	

			common::ui::tickerInfo newTicker;	
			newTicker.m_Message.Format(_T("%s"),common::utility::stringhelper::UTF8ToUnicode(tickerMessage).c_str());
			newTicker.m_Url.Format(_T("%s"),common::utility::stringhelper::UTF8ToUnicode(tickerUrl).c_str());		
			m_tickerVec.push_back(newTicker);

		}
		if(m_tickerVec.size()>0)
		{
			m_BottomTicker.RunTicker(m_tickerVec,100);   
		}

	}
	catch (...)
	{
		return;
	}

}

//设置为房间注册框
void CHallDlg::SetRoomRegister()
{
	if(m_RegisterDlg != NULL)
	{
		m_RegisterDlg->SetRoomRegister(true);
	}
}

//获取当前用户昵称
CString CHallDlg::GetCurNickName()
{
	CString nickName;
	core::CurInfo m_MyCurInfo = GetCurInfo();
	//昵称
	if(m_MyCurInfo.strName.size() == 0)  
	{
		nickName.Format(_T("%d"),m_MyCurInfo.un179id);

	}
	else
	{
		nickName.Format(_T("%s"),m_MyCurInfo.strName.c_str());		
	}

	return nickName;
}

void CHallDlg::SetRickLevelTip()
{
	core::CurInfo m_MyCurInfo = GetCurInfo();
	Room_Get_Rich_Level_Info info;
	info.unRichLevel = m_MyCurInfo.unRichLevel;
	g_HallModule->m_pModuleManager->CallService(Room::SERVICE_ROOM_GET_RICHLEVEL_INFO, (param)&info);


	Room_Get_Rich_Level_Info info2;
	info2.unRichLevel = m_MyCurInfo.unRichLevel+1;
	g_HallModule->m_pModuleManager->CallService(Room::SERVICE_ROOM_GET_RICHLEVEL_INFO, (param)&info2);

	CString strTemp;
	strTemp.Format(L"%s", info.strImgPath.c_str());
	m_pRichLevelBtn->SetGifBtnGifIcon(strTemp);
	CRect rcGifBtn;
	m_pRichLevelBtn->GetWindowRect(&rcGifBtn);
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(), L"imgRichLevel", L"width", (CComVariant)rcGifBtn.Width());
	m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(), L"imgRichLevel", L"height", (CComVariant)rcGifBtn.Height());
	CString strTip;	

	if(info.unRichLevel == 0) //富豪等级为0
	{
		strTip.Format(_T("快去消费吧!指挥辣妹跟你玩,还能升级哦!"));
	}
	else //富豪等级不为0
	{
		if(info2.strLevelName.size() != 0)
		{
			strTip.Format(_T("您现在是 %s,离升级到 %s 还差%u币"),info.strLevelName.c_str(),info2.strLevelName.c_str(),m_MyCurInfo.unRichUpdateMoney);
		}
		else //富豪等级已为最高级别
		{
			strTip.Format(_T("您现在是 %s"),info.strLevelName.c_str());
		}

	}

	m_pRichLevelBtn->SetGifBtnTipText(strTip);

	CComVariant var;
	m_pSkinEngine->GetSubSkinProp(GetSafeHwnd(), L"imgRichLevel", L"visible", &var);
	MoveGifButton(var.boolVal);
}
void CHallDlg::OnMoving(UINT fwSide, LPRECT pRect)
{
	CDialog::OnMoving(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
	MoveSafeCenterTipCtrl();
	CComVariant var;
	m_pSkinEngine->GetSubSkinProp(GetSafeHwnd(), L"imgRichLevel", L"visible", &var);
	MoveGifButton(var.boolVal);
}

void CHallDlg::DrawGifButton()
{
	SwitchResourceToModule(_T("Hall"));
	m_pRichLevelBtn = new common::ui::GifButton();
	CString strTemp = L"";
	if (m_pRichLevelBtn->CreateEx(this, strTemp))
	{
		//m_pRichLevelBtn->ShowGifBtn(rcClient);
		MoveGifButton(FALSE);
	}
}

void CHallDlg::MoveGifButton(BOOL bShow)
{
	if (m_pRichLevelBtn != NULL && m_pRichLevelBtn->GetSafeHwnd() != NULL)
	{
		if (!bShow)
		{
			m_pRichLevelBtn->ShowWindow(SW_HIDE);
		}else
		{
			CRect rc;
			m_pSkinEngine->GetSubSkinPos(GetSafeHwnd(), L"imgRichLevel", &(rc.left), &(rc.top), &(rc.right), &(rc.bottom));
			ClientToScreen(&rc);
			m_pRichLevelBtn->ShowGifBtn(rc);
		}
	}

	MoveSafeCenterTipCtrl();//调整安全中心tip位置

}

void CHallDlg::MoveSafeCenterTipCtrl()
{
	if ( m_pTipCtrl != NULL && m_pTipCtrl->GetSafeHwnd() )
	{
		int nWidth = 0 ;
		int nHeight = 0 ;
		CRect RectSafeSettig ;
		m_pSkinEngine->GetSubSkinPos(GetSafeHwnd(),L"btnSafeSetting",&(RectSafeSettig.left),&(RectSafeSettig.top),
			&(RectSafeSettig.right),&(RectSafeSettig.bottom));
		ClientToScreen( &RectSafeSettig ) ;
		CRect RectHeadCtrl ;
		m_pSkinEngine->GetSubSkinPos(GetSafeHwnd(),L"imgMyHeadCtrlBack_N",&(RectHeadCtrl.left),&(RectHeadCtrl.top),
			&(RectHeadCtrl.right),&(RectHeadCtrl.bottom));
		ClientToScreen( &RectHeadCtrl ) ;
		m_pTipCtrl->ShowTip( RectSafeSettig.left+RectSafeSettig.Width()/2- (RectHeadCtrl.right+5), nWidth , nHeight );
		m_pTipCtrl->MoveWindow(  RectHeadCtrl.right + 5, RectHeadCtrl.top+5 , nWidth ,nHeight ,TRUE ) ;
		//	m_pTipCtrl->SetWindowPos( NULL , RectHeadCtrl.right+5 , RectHeadCtrl.top+15 , nWidth ,nHeight , SWP_SHOWWINDOW|SWP_NOACTIVATE ) ;
	}
}

LRESULT CHallDlg::OnClickMsgBoxGoToRegister( WPARAM wParam, LPARAM lParam )
{
	if ((CHallDlg*)wParam == this)
	{
		OnClickBtnRegister(); 
	}

	return TRUE;
}
void CHallDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (IsWindowVisible())
	{
		CRect rcHead(0,0,0,0);
		m_pSkinEngine->GetSubSkinPos(GetSafeHwnd(), L"imgMyHeadCtrlBack_N", &(rcHead.left), &(rcHead.top), &(rcHead.right), &(rcHead.bottom));
		//ScreenToClient(&point);
		if (PtInRect(&rcHead, point))
		{
			CComVariant var = FALSE;
			m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(), L"imgMyHeadCtrlBack_N", L"visible", var);
			var = TRUE;
			m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(), L"imgMyHeadCtrlBack_H", L"visible", var);
		}
		else
		{
			CComVariant var;
			var = TRUE;
			m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(), L"imgMyHeadCtrlBack_N", L"visible", var);
			var = FALSE;
			m_pSkinEngine->SetSubSkinProp(GetSafeHwnd(), L"imgMyHeadCtrlBack_H", L"visible", var);
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}
