#include <BxScene.hpp>

class HelloWorldData
{
public:
	id_font font1;
public:
	HelloWorldData()
	{
		if(!BxCore::Font::IsExistNick("hoonr"))
			BxCore::Font::NickOpen("hoonr", "sys/hoonr.ttf");
		font1 = BxCore::Font::Open("hoonr", 100);

	}
	~HelloWorldData()
	{
		BxCore::Font::Close(font1);
	}
};

FRAMEWORK_SCENE(HelloWorldData, "HelloWorld")

syseventresult OnEvent(HelloWorldData& This, const sysevent& Event)
{
	if(Event.type == syseventtype_touch && Event.touch.type == systouchtype_up)
	{
		BxScene::SubRequest(__SCENE__, sceneside_left);
		BxScene::AddRequest("BxStartPage", sceneside_right, scenelayer_top, __SCENE__);
	}
	return syseventresult_pass;
}

sysupdateresult OnUpdate(HelloWorldData& This)
{
	return sysupdateresult_do_render;
}

void OnRender(HelloWorldData& This, BxDraw& Draw)
{
    Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(255, 255, 255));
	Draw.Text(This.font1, BxString("Hello world!"),
		Draw.CurrentCenter(), textalign_center_middle, COLOR(255, 0, 0));
}
