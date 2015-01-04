#include <BxScene.hpp>

class BxStartPage
{
public:
	id_font WinFont;
	BxImage WinFrameImage;
	BxImage WinPlayImage;
	BxString::Parse StartPage;
	BxString::Parse PushedPage;

public:
	BxStartPage()
	{
		if(!BxCore::Font::IsExistNick("nanum_gothic_extra_bold"))
			BxCore::Font::NickOpen("nanum_gothic_extra_bold", "sys/nanum_gothic_extra_bold.ttf");
		WinFont = BxCore::Font::Open("nanum_gothic_extra_bold", 12);
		WinFrameImage.Load("sys/win_frame.png", BxImage::PNG, BxImage::PAD7);
		WinPlayImage.Load("sys/win_play.png", BxImage::PNG, BxImage::PAD5);
	}

	~BxStartPage()
	{
		BxCore::Font::Close(WinFont);
	}
};

FRAMEWORK_SCENE(BxStartPage, "BxStartPage")

syseventresult OnEvent(BxStartPage& This, const sysevent& Event)
{
	if(Event.type == syseventtype_init)
		This.StartPage = Event.init.message;
	else if(Event.type == syseventtype_button)
	{
		if(Event.button.type == sysbuttontype_down)
			This.PushedPage = Event.button.name;
		else if(Event.button.type == sysbuttontype_drag)
		{
			if(Event.button.field.getsize().inside(Event.button.x, Event.button.y))
				This.PushedPage = Event.button.name;
			else This.PushedPage = "";
		}
		else if(Event.button.type == sysbuttontype_up && This.PushedPage.GetLength())
		{
			if(!BxScene::IsActivity(This.PushedPage))
			{
				BxScene::AddRequest(This.PushedPage);
				for(int i = 0, iend = BxScene::GetCount(); i < iend; ++i)
				{
					string OldScene = BxScene::GetName((scenelayer) i);
					BxScene::SubRequest(OldScene);
				}
			}
			This.PushedPage = "";
		}
		return syseventresult_done;
	}
	return syseventresult_pass;
}

sysupdateresult OnUpdate(BxStartPage& This)
{
	return sysupdateresult_do_render;
}

void OnRender(BxStartPage& This, BxDraw& Draw)
{
	Draw.Rectangle(FILL, XYXY(0, 0, Draw.Width(), 3), COLOR(104, 104, 104));
	Draw.Rectangle(FILL, XYXY(0, 3, 3, Draw.Height()), COLOR(123, 123, 123));
	Draw.Rectangle(FILL, XYXY(3, 3, Draw.Width(), Draw.Height()), COLOR(153, 153, 153));

	BxScene::AllScene* iScene = &BxScene::__GetAllScene__();
	const int iend = BxScene::__GetAllSceneCount__();
	const int MinGap = 20, FrameW = This.WinFrameImage.Width(), FrameH = This.WinFrameImage.Height();
	const int xend = Max(1, (Draw.Width() - MinGap) / (FrameW + MinGap));
	const int Gap = (Draw.Width() - FrameW * xend) / (xend + 1);
	for(int y = 0, i = 0; i < iend; ++y)
	for(int x = 0; x < xend && i < iend; ++x, ++i)
	{
		if(iScene->Next) iScene = iScene->Next;
		if(StrSameCount(iScene->Name, "Bx") == 2) --x;
		else BxTRY(Draw, CLIP(XYWH(Gap + (FrameW + Gap) * x, Gap + (FrameH + Gap) * y, FrameW, FrameH)))
		{
			// 스크린샷
			BxTRY(Draw, CLIP(XYXY(3, 23, Draw.Width() - 3, Draw.Height() - 17)))
				Draw.Rectangle(FILL, XYWH(0, 0, Draw.Width(), Draw.Height()), COLOR(128, 128, 128));
			// 프레임, 제목, 빌드날짜
			Draw.Area(0, 0, FORM(&This.WinFrameImage));
			BxTRY(Draw, CLIP(XYXY(3, 0, Draw.Width() - 3, 23)))
				Draw.TextByRect(This.WinFont, iScene->Name, XYWH(0, 0, Draw.Width(), Draw.Height()),
					textalign_center_middle, (StrCmp(iScene->Name, This.StartPage) == same)?
					COLOR(255, 128, 128) : COLOR(255, 255, 255));
			BxTRY(Draw, CLIP(XYXY(14, Draw.Height() - 17, Draw.Width() - 14, Draw.Height())))
				Draw.TextByRect(This.WinFont, iScene->BuildDate, XYWH(0, 0, Draw.Width(), Draw.Height()),
					textalign_center_middle, COLOR(255, 255, 255));
			// 플레이버튼
			BxTRY(Draw, CLIP(XYXY(3, 23, Draw.Width() - 3, Draw.Height() - 17)), iScene->Name)
			{
				bool IsPushed = (StrCmp(iScene->Name, This.PushedPage) == same);
				if(IsPushed) Draw.AddStyle(COLOR(255, 128, 128));
				Draw.Area(Draw.Width() / 2, Draw.Height() / 2, FORM(&This.WinPlayImage));
				if(IsPushed) Draw.SubStyle();
			}
		}
	}
}
