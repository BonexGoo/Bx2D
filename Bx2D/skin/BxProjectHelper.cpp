#include <BxScene.hpp>
#include <BxPanel.hpp>

class BxProjectHelper
{
public:
	id_font Font;
	BxPanel Edit_TargetProject;
	BxString Result_TargetProject;

public:
	BxProjectHelper()
	{
		if(!BxCore::Font::IsExistNick("hoonr"))
			BxCore::Font::NickOpen("hoonr", "sys/hoonr.ttf");
		Font = BxCore::Font::Open("hoonr", 18);

		Edit_TargetProject.Bind("BxIME.Edit", "|hoonr|18|Click me!|");
		Edit_TargetProject.Clicked += OnClicked_TargetProject, this;
		Edit_TargetProject.Updated += OnUpdated_TargetProject, this;
	}

	~BxProjectHelper()
	{
		BxCore::Font::Close(Font);
	}

	static void OnClicked_TargetProject(BxPanel* panel, string message, void* data)
	{
		BxProjectHelper* This = (BxProjectHelper*) data;
		string PtrString = This->Edit_TargetProject.Notify<string>("GetPtrString");
		BxScene::AddRequest("BxIME", sceneside_down, scenelayer_top, PtrString);
		This->Result_TargetProject = "";
	}

	static void OnUpdated_TargetProject(BxPanel* panel, string message, void* data)
	{
		BxProjectHelper* This = (BxProjectHelper*) data;
		if(StrCmp(message, "IMEDone") == same)
			This->Result_TargetProject = " (Matched)";
	}
};

FRAMEWORK_SCENE(BxProjectHelper, "BxProjectHelper")

syseventresult OnEvent(BxProjectHelper& This, const sysevent& Event)
{
	return syseventresult_pass;
}

sysupdateresult OnUpdate(BxProjectHelper& This)
{
	return sysupdateresult_do_render;
}

void OnRender(BxProjectHelper& This, BxDraw& Draw)
{
	BxTRY(Draw, COLOR(255, 0, 192, 10))
	{
		Draw.Rectangle(FILL, XYXY(0, 0, Draw.Width(), 3), COLOR(104, 104, 104));
		Draw.Rectangle(FILL, XYXY(0, 3, 3, Draw.Height()), COLOR(123, 123, 123));
		Draw.Rectangle(FILL, XYXY(3, 3, Draw.Width(), Draw.Height()), COLOR(153, 153, 153));
	}

	// 타겟프로젝트
	const int EndPosX = Draw.Text(This.Font, BxString("Target Project "), XY(20, 40),
		textalign_left_middle, COLOR(0, 0, 0)).x;
	BxTRY(Draw, CLIP(XYRR(EndPosX + 100, 40, 100, (BxCore::Font::GetHeight(This.Font) + 14) / 2)))
	{
		Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(0, 0, 0));
		Draw.Rectangle(FILL, XYXY(1, 1, Draw.Width() - 1, Draw.Height() - 1), COLOR(255, 255, 255));
		This.Edit_TargetProject.Paint(BxPanel::prmXYXY, BxARG(3, 3, Draw.Width() - 3, Draw.Height() - 3));
	}
	Draw.Text(This.Font, This.Result_TargetProject, XY(EndPosX + 200, 40),
		textalign_left_middle, COLOR(0, 64, 128));
}
