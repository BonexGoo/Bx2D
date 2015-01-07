#include <BxScene.hpp>
#include <BxPanel.hpp>

FRAMEWORK_SCENE_HIDDEN(int, "BxIME")

static int LastIndex;
static BxPanel* Board;

syseventresult OnEvent(int& Top, const sysevent& Event)
{
	if(Event.type == syseventtype_init)
	{
		Top = 0;
		LastIndex = -1;
		Board = BxNew(BxPanel);
		Board->Bind("fw.ime.IMEBoard", Event.init.message);
	}
	else if(Event.type == syseventtype_quit)
	{
		BxDelete(Board);
	}
	else if(Event.type == syseventtype_key)
	{
		int Index = -1;
		switch(Event.key.code)
		{
		case keykind_1: Index = 0; break; case keykind_2: Index = 1; break; case keykind_3: Index = 2; break;
		case keykind_4: Index = 3; break; case keykind_5: Index = 4; break; case keykind_6: Index = 5; break;
		case keykind_7: Index = 6; break; case keykind_8: Index = 7; break; case keykind_9: Index = 8; break;
		case keykind_0: Index = 9; break; case keykind_q: Index = 10; break; case keykind_w: Index = 11; break;
		case keykind_e: Index = 12; break; case keykind_r: Index = 13; break; case keykind_t: Index = 14; break;
		case keykind_y: Index = 15; break; case keykind_u: Index = 16; break; case keykind_i: Index = 17; break;
		case keykind_o: Index = 18; break; case keykind_p: Index = 19; break; case keykind_a: Index = 20; break;
		case keykind_s: Index = 21; break; case keykind_d: Index = 22; break; case keykind_f: Index = 23; break;
		case keykind_g: Index = 24; break; case keykind_h: Index = 25; break; case keykind_j: Index = 26; break;
		case keykind_k: Index = 27; break; case keykind_l: Index = 28; break; case keykind_shift: Index = 29; break;
		case keykind_z: Index = 30; break; case keykind_x: Index = 31; break; case keykind_c: Index = 32; break;
		case keykind_v: Index = 33; break; case keykind_b: Index = 34; break; case keykind_n: Index = 35; break;
		case keykind_m: Index = 36; break; case keykind_bspace: Index = 37; break; case keykind_ctrl: Index = 38; break;
		case keykind_space: Index = 39; break; case keykind_enter: Index = 40; break;
		}
		if(-1 < Index)
		{
			if(Event.key.type == syskeytype_down)
			{
				LastIndex = Index;
				Board->Notify("KeyDown", Index);
			}
			else if(Event.key.type == syskeytype_up && Index == LastIndex)
			{
				LastIndex = -1;
				Board->Notify("KeyUp", Index);
			}
		}
	}
	return syseventresult_pass;
}

sysupdateresult OnUpdate(int& Top)
{
	return sysupdateresult_do_render;
}

void OnRender(int& Top, BxDraw& Draw)
{
	Top = Board->Paint(BxPanel::prmXYXY, BxARG(0, 0, Draw.Width(), Draw.Height())).y;
}
