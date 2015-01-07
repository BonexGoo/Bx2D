#include "BxScene.hpp"

FRAMEWORK_SCENE_HIDDEN(unknown, "SceneSwap_C")

syseventresult OnEvent(unknown& This, const sysevent& Event)
{
	if(Event.type == syseventtype_touch && Event.touch.type == systouchtype_up)
	{
		BxScene::SubRequest(__SCENE__, sceneside_right);
		BxScene::AddRequest("SceneSwap_D", sceneside_left);
	}
	return syseventresult_pass;
}

sysupdateresult OnUpdate(unknown& This)
{
	return sysupdateresult_do_render;
}

void OnRender(unknown& This, BxDraw& Draw)
{
	Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(0, 255, 0));
}
