#include <BxPanel.hpp>

FRAMEWORK_PANEL("BxIME.Button", XYXY)

void OnCreate(string option, unknown param, const bool first)
{
	if(first)
	{
		s--.img[0].Load("sys/button.png", BxImage::PNG);
		s--.img[1].Load("sys/button_clicked.png", BxImage::PNG);
		s--.fnt("font").Load("sys/hoonr.ttf", 40);
        s--.int8("mode") = 0;
	}

	if(StrCmp(option, "SetMode") == same)
        s--.int8("mode") = (mint) param;
	else
	{
		m--.ptr[0] = (void*) ((wstring*) param)[0];
		m--.ptr[1] = (void*) ((wstring*) param)[1];
		m--.ptr[2] = (void*) ((wstring*) param)[2];
		m--.ptr[3] = (void*) ((wstring*) param)[3];
		m--.flag("IsClicked") = false;
		m--.flag("IsSpace") = (StrCmp(option, "space") == same);
		if(!m--.flag("IsSpace"))
			m--("Tip").Bind("BxIME.Tip", option);
	}
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "GetWChar") == same)
        return m--.ptr[s--.int8("mode")];
	else if(StrCmp(message, "KeyDown") == same) m--.DoTouch(systouchtype_down);
	else if(StrCmp(message, "KeyUp") == same) m--.DoTouch(systouchtype_up);
	return nullptr;
}

string OnTouch(BxPanel::Touch type)
{
	if(type == BxPanel::tchDown || type == BxPanel::tchMoveIn)
	{
		m--.flag("IsClicked") = true;
		if(m--("Tip").IsBinded())
            m--("Tip").Notify("SetString", m--.ptr[s--.int8("mode")]);
	}
	else if(type == BxPanel::tchMoveOut || type == BxPanel::tchUpIn || type == BxPanel::tchUpOut || type == BxPanel::tchUpAuto)
		 m--.flag("IsClicked") = false;
	return "";
}

void OnPaint(rect& r, int x1, int y1, int x2, int y2)
{
	BxDraw& Draw = m--.GetDraw();
	BxTRY(Draw, CLIP(r = XYXY(x1, y1, x2, y2)))
	{
		m--.UpdateRect();
		Draw.Rectangle9P(&s--.img[m--.flag("IsClicked")], Draw.CurrentRect());
        Draw.TextW(s--.fnt("font"), (wstring) m--.ptr[s--.int8("mode")],
			Draw.CurrentCenter(), textalign_center_middle,
			m--.flag("IsSpace")? COLOR(173, 180, 189) : COLOR(0, 0, 0));
	}
	if(m--("Tip").IsBinded() && m--.flag("IsClicked"))
		m--("Tip").Paint(BxPanel::prmXY, BxARG((x1 + x2) / 2, y1));
}
