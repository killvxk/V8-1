var cx;
var cy;

var SKIN_ON_CLICK_CLOSE = 1;
var SKIN_ON_CLICK_OK = 2;

Window.AttachEvent("OnInit", OnInit);
Window.AttachEvent("OnSize", OnSize);
Window.AttachEvent("OnNotify", OnNotify);

btnClose.AttachEvent("OnClick", OnClickBtnClose);
btnOK.AttachEvent("OnClick", OnClickBtnOK);

function OnClickBtnClose()
{
	Window.PostSkinMessage(SKIN_ON_CLICK_CLOSE, 0);
}

function OnClickBtnOK()
{
	Window.PostSkinMessage(SKIN_ON_CLICK_OK, 0);
}

function OnInit()
{
    btnSysClose.visible = false;
	OnSize(Window.width, Window.height);
}

function OnSize(cx, cy)
{	
		Window.BeginPaint();
		//取消圆角及圆角外框
		Window.upCornerRadius = "0";
		Window.downCornerRadius = "0";
		imgFrameBackground.visible = false;
		
		//加入直角外框，背景变白，关闭按钮，确定取消按钮
		imgSpotSongTop.visible = true;
		imgSpotSongTop.left = 0;
		imgSpotSongTop.top = 0;
		imgSpotSongTop.width = cx;
		imgSpotSongTop.height = 1;
		
		imgSpotSongLeft.visible = true;
		imgSpotSongLeft.left = 0;
		imgSpotSongLeft.top = 0;
		imgSpotSongLeft.width = 1;
		imgSpotSongLeft.height = cy;
		
		imgSpotSongRight.visible = true;
		imgSpotSongRight.left = cx - 1;
		imgSpotSongRight.top = imgSpotSongLeft.top;
		imgSpotSongRight.width = imgSpotSongLeft.width;
		imgSpotSongRight.height = imgSpotSongLeft.height;
		
		imgSpotSongBottom.visible = true;
		imgSpotSongBottom.left = imgSpotSongTop.left;
		imgSpotSongBottom.top = imgSpotSongLeft.top + imgSpotSongLeft.height - 1;
		imgSpotSongBottom.width = imgSpotSongTop.width;
		imgSpotSongBottom.height = 1;
		
		imgSpotSongBk.left = 1;
		imgSpotSongBk.top = 1;
		imgSpotSongBk.width = cx - 2;
		imgSpotSongBk.height = cy - 2;	
		
		btnClose.left = cx - 2 - btnClose.width;
		btnClose.top = 2;
		
		textPrompt1.left = 35;
		textPrompt1.top = 30;
		textPrompt1.width = textPrompt1.textWidth;
		textPrompt1.height = textPrompt1.textHeight;
		
		imgEditNumBk.left = textPrompt1.left + textPrompt1.width;
		imgEditNumBk.top = textPrompt1.top - 4;
		imgEditNumBk.width = 40;
		imgEditNumBk.height = 25;
		
		itemEditNum.left = imgEditNumBk.left + 2;
		itemEditNum.top = imgEditNumBk.top + 6;
		itemEditNum.width = imgEditNumBk.width - 4;
		itemEditNum.height = imgEditNumBk.height - 10;
		
		textPrompt2.left = imgEditNumBk.left + imgEditNumBk.width;
		textPrompt2.top = textPrompt1.top;
		textPrompt2.width = textPrompt2.textWidth;
		textPrompt2.height = textPrompt2.textHeight;
		
		btnOK.left = textPrompt2.left + textPrompt2.width + 40;
		btnOK.top = textPrompt2.top - 3;
		
		textNoEnoughMoneyLeft.left = 10;
		textNoEnoughMoneyLeft.top = textPrompt1.top + textPrompt1.height + 15;
		textNoEnoughMoneyLeft.width = textNoEnoughMoneyLeft.textWidth;
		textNoEnoughMoneyLeft.height = textNoEnoughMoneyLeft.textHeight;
		
		textNoEnoughMoneyMid.left = textNoEnoughMoneyLeft.left + textNoEnoughMoneyLeft.width;
		textNoEnoughMoneyMid.top = textNoEnoughMoneyLeft.top;
		textNoEnoughMoneyMid.width = 15;
		textNoEnoughMoneyMid.height = textNoEnoughMoneyLeft.height;
		
		textNoEnoughMoneyRight.left = textNoEnoughMoneyMid.left + textNoEnoughMoneyMid.width;
		textNoEnoughMoneyRight.top = textNoEnoughMoneyMid.top;
		textNoEnoughMoneyRight.width = textNoEnoughMoneyRight.textWidth;
		textNoEnoughMoneyRight.height = textNoEnoughMoneyRight.textHeight;
		
		textWarmTip1.left = textNoEnoughMoneyLeft.left;
		textWarmTip1.top = textNoEnoughMoneyLeft.top + textNoEnoughMoneyLeft.height + 12;
		textWarmTip1.width = textWarmTip1.textWidth;
		textWarmTip1.height = textWarmTip1.textHeight;
		
		textWarmTip2Left.left = textWarmTip1.left + 60;
		textWarmTip2Left.top = textWarmTip1.top + textWarmTip1.height + 4;
		textWarmTip2Left.width = textWarmTip2Left.textWidth;
		textWarmTip2Left.height = textWarmTip2Left.textHeight;
		
		textWarmTip2Mid.left = textWarmTip2Left.left + textWarmTip2Left.width;
		textWarmTip2Mid.top = textWarmTip2Left.top;
		textWarmTip2Mid.width = 50;
		textWarmTip2Mid.height = textWarmTip2Left.height;
		
		textWarmTip2Right.left = textWarmTip2Mid.left + textWarmTip2Mid.width;
		textWarmTip2Right.top = textWarmTip2Mid.top;
		textWarmTip2Right.width = textWarmTip2Right.textWidth;
		textWarmTip2Right.height = textWarmTip2Right.textHeight;
		
		textWarmTip3.left = textWarmTip2Right.left + textWarmTip2Right.width + 12;
		textWarmTip3.top = textWarmTip2Right.top;
		textWarmTip3.width = textWarmTip3.textWidth;
		textWarmTip3.height = textWarmTip3.textHeight;
	
		Window.EndPaint();
 
}

function OnNotify(code, code1)
{
  switch(code)
  {

  default:
    break;
  }
}


