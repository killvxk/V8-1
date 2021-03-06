
var SKIN_ON_CLICK_CLOSE = 1;
var SKIN_ON_CLICK_OK = 2;

Window.AttachEvent("OnInit", OnInit);
Window.AttachEvent("OnSize", OnSize);
Window.AttachEvent("OnNotify", OnNotify);

btnSysClose.AttachEvent("OnClick", OnClickbtnClose);

btnOK.AttachEvent("OnClick", OnClickOK);
btnCancel.AttachEvent("OnClick", OnClickbtnClose);

function OnClickbtnClose()
{
	Window.PostSkinMessage(SKIN_ON_CLICK_CLOSE, 0);
}

function OnClickOK()
{
	Window.PostSkinMessage(SKIN_ON_CLICK_OK, 0);
}

function OnInit()
{
	textTitle.text = "点歌祝福";
    textTitle.visible = false;
	Window.upCornerRadius = "0";
	Window.downCornerRadius = "0";
	imgFrameBackground.visible = false;
	
	OnSize(Window.width, Window.height);
}

function OnSize(cx, cy)
{	
	imgSongWishTop.visible = true;
	imgSongWishTop.left = 0;
	imgSongWishTop.top = 0;
	imgSongWishTop.width = cx;
	imgSongWishTop.height = 2;
		
	imgSongWishLeft.visible = true;
	imgSongWishLeft.left = 0;
	imgSongWishLeft.top = 2;
	imgSongWishLeft.width = 1;
	imgSongWishLeft.height = cy - 2;
		
	imgSongWishRight.visible = true;
	imgSongWishRight.left = cx - 1;
	imgSongWishRight.top = imgSongWishLeft.top;
	imgSongWishRight.width = imgSongWishLeft.width;
	imgSongWishRight.height = imgSongWishLeft.height;
		
	imgSongWishBottom.visible = true;
	imgSongWishBottom.left = imgSongWishTop.left;
	imgSongWishBottom.top = imgSongWishLeft.top + imgSongWishLeft.height - 1;
	imgSongWishBottom.width = imgSongWishTop.width;
	imgSongWishBottom.height = 1;
		
	imgSongWishDlgBk.left = 1;
	imgSongWishDlgBk.top = 2;
	imgSongWishDlgBk.width = cx - 2;
	imgSongWishDlgBk.height = cy - 3;
	
	btnSysClose.normalImage = "Room_OrderSongMsgBox_btnClose_N.png";
	btnSysClose.hoverImage="Room_OrderSongMsgBox_btnClose_H.png";
	btnSysClose.downImage="Room_OrderSongMsgBox_btnClose_H.png";
	btnSysClose.top = 8;
	btnSysClose.left = cx - 8 - btnSysClose.width;
	
	textTheTitle.left = 10;
	textTheTitle.top = 8;
	textTheTitle.width = textTheTitle.textWidth;
	textTheTitle.height = textTheTitle.textHeight;
	
	textPrompt.visible = true;
	textPrompt.left = 13;
	textPrompt.top = 16;
	textPrompt.width = textPrompt.textWidth;
	textPrompt.height = textPrompt.textHeight;
	
	textSong.visible = true;
	textSong.left = textPrompt.left + textPrompt.width;
	textSong.top = textPrompt.top;
	textSong.width = cx - 2 - textSong.left;
	textSong.height = textPrompt.height;
	
	textToSay.visible = true;
	textToSay.left = textPrompt.left;
	textToSay.top = textPrompt.top + textPrompt.height + 8;
	textToSay.width = textToSay.textWidth;
	textToSay.height = textToSay.textHeight;
	
	imgInputRound.left = textPrompt.left;
	imgInputRound.top = textToSay.top + textToSay.height + 8;
	imgInputRound.width = cx - 13 * 2;
	imgInputRound.height = 80;
	
	itemSongWishInput.left = imgInputRound.left + 4;
	itemSongWishInput.top = imgInputRound.top + 4;
	itemSongWishInput.width = imgInputRound.width - 8;
	itemSongWishInput.height = imgInputRound.height - 8;
	
	textTheMostNum.width = textTheMostNum.textWidth;
	textTheMostNum.height = textTheMostNum.textHeight;
	textTheMostNum.left = imgInputRound.left + imgInputRound.width - textTheMostNum.width;
	textTheMostNum.top = imgInputRound.top + imgInputRound.height + 10;
	
	btnOK.visible = true;
	btnOK.width = 70;
	btnOK.height = 25;
	btnOK.left = imgInputRound.left;
	btnOK.top = textTheMostNum.top;
	
	btnCancel.visible = true;
	btnCancel.width = btnOK.width;
	btnCancel.height = btnOK.height;
	btnCancel.left = btnOK.left + btnOK.width + 8;
	btnCancel.top = btnOK.top;
	/////////////////////////////////////////////////
	
}

function OnNotify(code, code1)
{
	switch(code)
	{
		default:
			break;
	}
}
