#include "BxScene.hpp"

FRAMEWORK_SCENE(int, "A")

syseventresult OnEvent(int& This, const sysevent& Event)
{
	if(Event.type == syseventtype_touch && Event.touch.type == systouchtype_up)
	{
		BxScene::SubRequest("A", sceneside_left);
		BxScene::AddRequest("B", sceneside_right);
	}
	return syseventresult_pass;
}

sysupdateresult OnUpdate(int& This)
{
	return sysupdateresult_do_render;
}

void OnRender(int& This, BxDraw& Draw)
{
	Draw.Rectangle(FILL, XYWH(0, 0, Draw.Width(), Draw.Height()), COLOR(255, 0, 0));
}
