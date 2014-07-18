Window.AttachEvent("OnInit", OnInit);
Window.AttachEvent("OnSize", OnSize);
Window.AttachEvent("OnNotify", OnNotify);

function OnInit()
{
	OnSize(Window.width, Window.height);
}

function OnSize(cx, cy)
{
	Window.BeginPaint();
	 // imgTabSettingBackground.left = 0;
	 // imgTabSettingBackground.top = 0;
	 // imgTabSettingBackground.width = cx;
	 // imgTabSettingBackground.height = cy;
	 imgTop.left = 0;
	 imgTop.top = 0;
	 imgTop.width = cx;
	 imgTop.height = 1;
	 
	 imgBottom.left = imgTop.left;
	 imgBottom.top = cy - 1;
	 imgBottom.width = imgTop.width;
	 imgBottom.height = imgTop.height;
	 
	 imgLeft.left = 0;
	 imgLeft.top = 0;
	 imgLeft.width = 1;
	 imgLeft.height = cy;
	 
	 imgRight.left = cx - 1;
     imgRight.top = imgLeft.top;
     imgRight.width = imgLeft.width;
     imgRight.height = imgLeft.height;	 
	 
	
	Window.EndPaint();
}

function OnNotify(code, code1)
{
}
