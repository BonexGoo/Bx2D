#include "BxScene.hpp"

FRAMEWORK_SCENE(unknown, "SceneSwap_A")

syseventresult OnEvent(unknown& This, const sysevent& Event)
{
	if(Event.type == syseventtype_touch && Event.touch.type == systouchtype_up)
	{
		BxScene::SubRequest(__SCENE__, sceneside_left);
		BxScene::AddRequest("SceneSwap_B", sceneside_right);
	}
	return syseventresult_pass;
}

sysupdateresult OnUpdate(unknown& This)
{
	return sysupdateresult_do_render;
}

void OnRender(unknown& This, BxDraw& Draw)
{
	Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(255, 0, 0));
}
