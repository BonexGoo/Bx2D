#include <BxPanel.hpp>

FRAMEWORK_PANEL("BxIME.Tip", XY)

void OnCreate(string option, unknown param, const bool first)
{
	if(first)
	{
		s--.img[0].Load("sys/tip_left.png", BxImage::PNG);
		s--.img[1].Load("sys/tip_mid.png", BxImage::PNG);
		s--.img[2].Load("sys/tip_right.png", BxImage::PNG);
		s--.fnt("font").Load("sys/hoonr.ttf", 58);
	}
	m--.ptr[0] = (void*) W2("");
	if(StrCmp(option, "left") == same) m--.int4[0] = 0;
	else if(StrCmp(option, "mid") == same) m--.int4[0] = 1;
	else if(StrCmp(option, "right") == same) m--.int4[0] = 2;
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "SetString") == same)
		m--.ptr[0] = (void*) param;
	return nullptr;
}

string OnTouch(BxPanel::Touch type, int x, int y)
{
	return "";
}

void OnPaint(rect& r, int x, int y, int, int)
{
	BxDraw& Draw = m--.GetDraw();
	const int Index = m--.int4[0];
	const int Width = s--.img[Index].Width();
	const int Height = s--.img[Index].Height();
	BxTRY(Draw, CLIP(r = XYWH(x - Width / 2, y - 70, Width, Height)))
	{
		Draw.Area(0, 0, FORM(&s--.img[Index]));
		Draw.TextW(s--.fnt("font"), (wstring) m--.ptr[0],
			XY(Draw.Width() / 2, (Draw.Height() - 10) / 2), textalign_center_middle, COLOR(0, 0, 0));
	}
}
