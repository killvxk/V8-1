var SKIN_ON_CLICK_BALANCE = 1;
var SKIN_ON_CLICK_DEPOSIT = 2;
var SKIN_ON_CLICK_CHECK_AND_WITHDRAWL = 3;
var SKIN_ON_CLICK_CLOSE = 4;

var NOTIFY_SKIN_CLICK_DEPOSIT = 5;
var NOTIFY_SKIN_CLICK_CHECK_AND_WITHDRAWL = 6;
var NOTIFY_SKIN_CLICK_BALANCE = 7;

Window.AttachEvent("OnInit", OnInit);
Window.AttachEvent("OnSize", OnSize);
Window.AttachEvent("OnNotify", OnNotify);

textItemBalance.AttachEvent("OnClick",OnClickBalance);
textItemDeposit.AttachEvent("OnClick",OnClickDeposit);
textItemCheckAndWithdrawl.AttachEvent("OnClick",OnClickCheckAndWithdrawl);

btnSysClose.AttachEvent("OnClick",OnClickClose);

function OnClickClose()
{
    Window.PostSkinMessage(SKIN_ON_CLICK_CLOSE,0);
}

function OnClickBalance()
{
    imgBalanceBk.visible = true;
	imgDepositBk.visible = false;
	imgCheckAndWithdrawlBk.visible = false;
	
	itemShowBalanceDlg.visible = true;
	itemShowDepositDlg.visible = false;
	itemShowCheckAndWithdrawlDlg.visible = false;
	Window.PostSkinMessage(SKIN_ON_CLICK_BALANCE,0);
}

function OnClickDeposit()
{
    imgBalanceBk.visible = false;
	imgDepositBk.visible = true;
	imgCheckAndWithdrawlBk.visible = false;
	
	itemShowBalanceDlg.visible = false;
	itemShowDepositDlg.visible = true;
	itemShowCheckAndWithdrawlDlg.visible = false;
    Window.PostSkinMessage(SKIN_ON_CLICK_DEPOSIT,0);
}

function OnClickCheckAndWithdrawl()
{
    imgBalanceBk.visible = false;
	imgDepositBk.visible = false;
	imgCheckAndWithdrawlBk.visible = true;
	
	itemShowBalanceDlg.visible = false;
	itemShowDepositDlg.visible = false;
	itemShowCheckAndWithdrawlDlg.visible = true;
	Window.PostSkinMessage(SKIN_ON_CLICK_CHECK_AND_WITHDRAWL,0);
}

function OnInit()
{
	textTitle.text = "保险箱";
    textTitle.fontSize="10";
	
	
	OnSize(Window.width, Window.height);
}

function OnSize(cx, cy)
{	

	imgSafeBoxBk.visible = true;
	imgSafeBoxBk.left = 1;
	imgSafeBoxBk.top = textTitle.top + textTitle.height + 8;
	imgSafeBoxBk.width = cx - 2;
	imgSafeBoxBk.height = cy - textTitle.height - 42;
	
	imgBorder.visible = true;
	imgBorder.left = imgSafeBoxBk.left + 5;
	imgBorder.top = imgSafeBoxBk.top + 3;
	imgBorder.width = 120;
	imgBorder.height = imgSafeBoxBk.height - 10;
	
	imgBalanceBk.visible = true;
	imgBalanceBk.left = imgBorder.left + 1;
	imgBalanceBk.top = imgBorder.top + 10;
	imgBalanceBk.width = imgBorder.width - 2;
	imgBalanceBk.height = 20;
	textItemBalance.visible = true;
	textItemBalance.left = imgBalanceBk.left + 15;
	textItemBalance.top = imgBalanceBk.top + 2;
	textItemBalance.width = textItemBalance.textWidth;
	textItemBalance.height = textItemBalance.textHeight;
	
	imgDepositBk.visible = false;
	imgDepositBk.left = imgBalanceBk.left;
	imgDepositBk.top = imgBalanceBk.top + imgBalanceBk.height + 10;
	imgDepositBk.width = imgBalanceBk.width;
	imgDepositBk.height = imgBalanceBk.height;
	textItemDeposit.visible = true;
	textItemDeposit.left = imgDepositBk.left + 15;
	textItemDeposit.top = imgDepositBk.top + 2;
	textItemDeposit.width = textItemDeposit.textWidth;
	textItemDeposit.height = textItemDeposit.textHeight;
	
	imgCheckAndWithdrawlBk.visible = false;
	imgCheckAndWithdrawlBk.left = imgDepositBk.left;
	imgCheckAndWithdrawlBk.top = imgDepositBk.top + imgDepositBk.height + 10;
	imgCheckAndWithdrawlBk.width = imgDepositBk.width;
	imgCheckAndWithdrawlBk.height = imgDepositBk.height;
	textItemCheckAndWithdrawl.visible = true;
	textItemCheckAndWithdrawl.left = imgCheckAndWithdrawlBk.left + 15;
	textItemCheckAndWithdrawl.top = imgCheckAndWithdrawlBk.top + 2;
	textItemCheckAndWithdrawl.width = textItemCheckAndWithdrawl.textWidth;
	textItemCheckAndWithdrawl.height = textItemCheckAndWithdrawl.textHeight;
	
	/*以下为右边子窗口*/
	/////////账户余额////////
	itemShowBalanceDlg.visible = true;
	itemShowBalanceDlg.left = imgBorder.left + imgBorder.width + 40;
	itemShowBalanceDlg.top = imgBorder.top - 4;
	itemShowBalanceDlg.width = 430;
	itemShowBalanceDlg.height = 310;
	/////////存款///////////
	itemShowDepositDlg.visible = false;
	itemShowDepositDlg.left = imgBorder.left + imgBorder.width + 40;
	itemShowDepositDlg.top = imgBorder.top - 4;
	itemShowDepositDlg.width = 430;
	itemShowDepositDlg.height = 310;
	/////////查询-取款///////
	itemShowCheckAndWithdrawlDlg.visible = false;
	itemShowCheckAndWithdrawlDlg.left = imgBorder.left + imgBorder.width + 40;
	itemShowCheckAndWithdrawlDlg.top = imgBorder.top - 4;
	itemShowCheckAndWithdrawlDlg.width = 430;
	itemShowCheckAndWithdrawlDlg.height = 310;
	

}

function OnNotify(code, code1)
{
	switch(code)
	{
	  case NOTIFY_SKIN_CLICK_DEPOSIT:
	       OnClickDeposit();
	       break;
	  case NOTIFY_SKIN_CLICK_CHECK_AND_WITHDRAWL:
	       OnClickCheckAndWithdrawl();
	       break;
	  case NOTIFY_SKIN_CLICK_BALANCE:
	       OnClickBalance();
		   break;
	  default:break;
	}
}
