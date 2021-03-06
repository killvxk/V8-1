
var SKIN_ON_CLICK_BTN_CONFIRM_SAVE  = 1;

var SKIN_ON_CLICK_JIACU = 2;
var SKIN_ON_CLICK_XIETI = 3;
var SKIN_ON_CLICK_UNDERLINE = 4;
var SKIN_ON_CLICK_FONT_COLOR = 5;
var SKIN_ON_CLICK_BTN_EMOTION = 6;

Window.AttachEvent("OnInit", OnInit);
Window.AttachEvent("OnSize", OnSize);
Window.AttachEvent("OnNotify", OnNotify);

//btnConfirmSave.AttachEvent("OnClick",OnClickBtnConfirmSave);

btnJiaCu.AttachEvent("OnClick",OnClickBtnJiaCu);
btnXieTi.AttachEvent("OnClick",OnClickBtnXieTi);
btnUnderLine.AttachEvent("OnClick",OnClickBtnUnderLine);
btnColor.AttachEvent("OnClick",OnClickBtnColor);
// btnEmotion.AttachEvent("OnClick",OnClickBtnEmotion);

// function OnClickBtnConfirmSave()
// {
    // Window.PostSkinMessage(SKIN_ON_CLICK_BTN_CONFIRM_SAVE,0);
// }

function OnClickBtnJiaCu()
{
   if(false == btnJiaCu.checked)
   {
      btnJiaCu.checked = true;
	  Window.PostSkinMessage(SKIN_ON_CLICK_JIACU,1);
   }else
   {
      btnJiaCu.checked = false;
	  Window.PostSkinMessage(SKIN_ON_CLICK_JIACU,0);
   }
}

function OnClickBtnXieTi()
{
   if(false == btnXieTi.checked)
   {
      btnXieTi.checked = true;
	  Window.PostSkinMessage(SKIN_ON_CLICK_XIETI,1);
   }else
   {
      btnXieTi.checked = false;
	  Window.PostSkinMessage(SKIN_ON_CLICK_XIETI,0);
   }
}

function OnClickBtnUnderLine()
{
   if(false == btnUnderLine.checked)
   {
      btnUnderLine.checked = true;
	  Window.PostSkinMessage(SKIN_ON_CLICK_UNDERLINE,1);
   }else
   {
      btnUnderLine.checked = false;
	  Window.PostSkinMessage(SKIN_ON_CLICK_UNDERLINE,0);
   }
}

function OnClickBtnColor()
{
   Window.PostSkinMessage(SKIN_ON_CLICK_FONT_COLOR,0);
}

// function OnClickBtnEmotion()
// {
 // Window.PostSkinMessage(SKIN_ON_CLICK_BTN_EMOTION,0);
// }


function OnInit()
{
	OnSize(Window.width, Window.height);
}


function OnSize(cx, cy)
{

	imgUserMgrDlgBk.visible = true;
	imgUserMgrDlgBk.left = 0;
	imgUserMgrDlgBk.top = 0;
	imgUserMgrDlgBk.width = cx;
	imgUserMgrDlgBk.height = cy;
	
	// UserMgrDlgTitleEffect.visible = false;
	// UserMgrDlgTitleEffect.left = 0; 
	// UserMgrDlgTitleEffect.top = 0;
	
	imgTitleIcon.visible = true;
	imgTitleIcon.left = 10;
	imgTitleIcon.top = 1;

	textTheTitle.visible = true;
	textTheTitle.left = imgTitleIcon.left + imgTitleIcon.width + 5;
	textTheTitle.top = 4;
	textTheTitle.width = textTheTitle.textWidth;
	textTheTitle.height = textTheTitle.textHeight;
	
	imgTitleLine.visible = true;
	imgTitleLine.left = textTheTitle.left + textTheTitle.width + 2;
	imgTitleLine.top = textTheTitle.top +(textTheTitle.height - imgTitleLine.height)/2;
	////////////////////////
	
	radioSysWelcomeWord.visible = true;
	radioSysWelcomeWord.left = textTheTitle.left + 25;
	radioSysWelcomeWord.top = textTheTitle.top + textTheTitle.height + 70;
	
	textSysWelcomWord.visible = true;
	textSysWelcomWord.left = radioSysWelcomeWord.left + 20;
	textSysWelcomWord.top = radioSysWelcomeWord.top + radioSysWelcomeWord.height + 15;
	textSysWelcomWord.width = textSysWelcomWord.textWidth;
	textSysWelcomWord.height = textSysWelcomWord.textHeight;
	
	radioDefineWelcomeWord.visible = true;
	radioDefineWelcomeWord.left = radioSysWelcomeWord.left;
	radioDefineWelcomeWord.top = textSysWelcomWord.top + textSysWelcomWord.height + 15;
	
	imgHtmlBound.visible = true;
	imgHtmlBound.left = textTheTitle.left + 25;
	imgHtmlBound.top = radioDefineWelcomeWord.top + 25;
	imgHtmlBound.width = imgUserMgrDlgBk.width - 120;
	imgHtmlBound.height = 120;
	
	imgFontToolBar.visible = true;
	imgFontToolBar.left = imgHtmlBound.left + 1; 
	imgFontToolBar.top = imgHtmlBound.top + 1;
	imgFontToolBar.width = imgHtmlBound.width - 2;
	imgFontToolBar.height = 28;
	
	btnJiaCu.left = imgFontToolBar.left + 2;
	btnJiaCu.top = imgFontToolBar.top + 2;
	// btnJiaCu.width = 24;
	// btnJiaCu.height = imgFontToolBar.height - 2;
	
	btnXieTi.left = btnJiaCu.left + btnJiaCu.width + 3;
	btnXieTi.top = btnJiaCu.top;
	// btnXieTi.width = btnJiaCu.width;
	// btnXieTi.height = btnJiaCu.height;
	
	btnUnderLine.left = btnXieTi.left + btnXieTi.width + 3;
	btnUnderLine.top = btnXieTi.top;
	// btnUnderLine.width = btnXieTi.width;
	// btnUnderLine.height = btnXieTi.height;

    btnColor.left = btnUnderLine.left + btnUnderLine.width + 3;
    btnColor.top = btnUnderLine.top;
	btnColor.width = btnUnderLine.width;
	btnColor.height = btnUnderLine.height;
	
	itemComboFontSize.left = btnColor.left + btnColor.width + 5;
	itemComboFontSize.top = btnColor.top;
	itemComboFontSize.width = 50;
	itemComboFontSize.height = btnColor.height;
	
	itemComboFont.left = itemComboFontSize.left + itemComboFontSize.width + 5;
	itemComboFont.top = itemComboFontSize.top;
	itemComboFont.width = 80;
	itemComboFont.height = itemComboFontSize.height;
	
	// btnEmotion.visible = true;
	// btnEmotion.left = itemComboFont.left + itemComboFont.width + 5;
	// btnEmotion.top = itemComboFont.top;
	// btnEmotion.width = 50;
	// btnEmotion.height = 22;
	
	itemHtmlContent.visible = true;
	itemHtmlContent.left =  imgFontToolBar.left;
	itemHtmlContent.top = imgFontToolBar.top + imgFontToolBar.height;
	itemHtmlContent.width = imgFontToolBar.width;
	itemHtmlContent.height = imgHtmlBound.top + imgHtmlBound.height - 4 - itemHtmlContent.top;
	
	// btnConfirmSave.visible = true;
	// btnConfirmSave.width = 77;
	// btnConfirmSave.height = 30;
	// btnConfirmSave.left = itemHtmlContent.left + (itemHtmlContent.width - btnConfirmSave.width)/2 - 10;
	// btnConfirmSave.top = itemHtmlContent.top + itemHtmlContent.height + 20;
	
	
	imgApplyEffect.left = 0;
	imgApplyEffect.top = 0;
	imgApplyEffect.width = cx;
	imgApplyEffect.height = cy;
	
}

function OnNotify(code, code1)
{
  switch(code)
  {
 
  default:
    break;
  }
}
