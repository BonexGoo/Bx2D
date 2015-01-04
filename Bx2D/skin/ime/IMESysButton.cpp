#include <BxPanel.hpp>

FRAMEWORK_PANEL("fw.ime.IMESysButton", XYXY)

void OnCreate(string option, unknown param, const bool first)
{
	if(first)
	{
		s--.img[0].Load("sys/sysbutton.png", BxImage::PNG);
		s--.img[1].Load("sys/sysbutton_clicked.png", BxImage::PNG);
	}

	if(StrCmp(option, "Shift") == same)
	{
		m--.img[0].Load("sys/shift.png", BxImage::PNG, BxImage::PAD5);
		m--.img[1].Load("sys/shift_selected.png", BxImage::PNG, BxImage::PAD5);
		m--.flag("IsAutoBright") = false;
	}
	else if(StrCmp(option, "BSpace") == same)
	{
		m--.img[0].Load("sys/bspace.png", BxImage::PNG, BxImage::PAD5);
		m--.img[1].Load("sys/bspace_bright.png", BxImage::PNG, BxImage::PAD5);
		m--.flag("IsAutoBright") = true;
	}
	else if(StrCmp(option, "Language") == same)
	{
		m--.img[0].Load("sys/english.png", BxImage::PNG, BxImage::PAD5);
		m--.img[1].Load("sys/korean.png", BxImage::PNG, BxImage::PAD5);
		m--.flag("IsAutoBright") = false;
	}
	else if(StrCmp(option, "Done") == same)
	{
		m--.img[0].Load("sys/done.png", BxImage::PNG, BxImage::PAD5);
		m--.img[1].Load("sys/done_bright.png", BxImage::PNG, BxImage::PAD5);
		m--.flag("IsAutoBright") = true;
	}
	m--.flag("IsClicked") = false;
	m--.flag("IsBrighted") = false;
	m--.str("TypeName") = option;
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "KeyDown") == same) m--.DoTouch(systouchtype_down);
	else if(StrCmp(message, "KeyUp") == same) m--.DoTouch(systouchtype_up);
	else if(StrCmp(message, "SetBright") == same)
		m--.flag("IsBrighted") = (bool) param;
	return nullptr;
}

string OnTouch(BxPanel::Touch type)
{
	if(type == BxPanel::tchDown || type == BxPanel::tchMoveIn) m--.flag("IsClicked") = true;
	else if(type == BxPanel::tchMoveOut || type == BxPanel::tchUpIn || type == BxPanel::tchUpOut || type == BxPanel::tchUpAuto)
		 m--.flag("IsClicked") = false;
	if(m--.flag("IsAutoBright"))
		m--.flag("IsBrighted") = m--.flag("IsClicked");
	return m--.str("TypeName");
}

void OnPaint(rect& r, int x1, int y1, int x2, int y2)
{
	BxDraw& Draw = m--.GetDraw();
	BxTRY(Draw, CLIP(r = XYXY(x1, y1, x2, y2)))
	{
		m--.UpdateRect();
		Draw.Rectangle9P(&s--.img[m--.flag("IsClicked")],
			XYWH(0, 0, Draw.Width(), Draw.Height()));
		Draw.Area(Draw.Width() / 2, Draw.Height() / 2, FORM(&m--.img[m--.flag("IsBrighted")]));
	}
}
