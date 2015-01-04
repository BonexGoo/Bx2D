#pragma once
#include <BxDraw.hpp>
#include <BxPanel.hpp>
#include <BxString.hpp>
using namespace BxUtilGlobal;
using namespace BxDrawGlobal;

enum sceneside:int {sceneside_current = -1, sceneside_center, sceneside_up, sceneside_down, sceneside_left, sceneside_right};
enum scenelayer:int {scenelayer_current = -3, scenelayer_gui = -2, scenelayer_top = -1,
	scenelayer_1st = 0, scenelayer_2nd = 1, scenelayer_3rd = 2, scenelayer_4th = 3, scenelayer_5th = 4,
	scenelayer_6th = 5, scenelayer_7th = 6, scenelayer_8th = 7, scenelayer_9th = 8, scenelayer_10th = 9};
enum sceneflow:byte {sceneflow_1x, sceneflow_2x, sceneflow_4x, sceneflow_accel, sceneflow_decel};

//! \brief 씬관리
class BxScene
{
public:
	/*!
	\brief 씬활성화 요청
	\param Name : 씬이름
	\param Side : 등장할 방향
	\param Layer : 삽입할 레이어
	\param Message : 초기화 메시지
	\see SubRequest
	*/
	global_func void AddRequest(string Name, sceneside Side = sceneside_center, scenelayer Layer = scenelayer_top, string Message = "")
	{
		BxASSERT("BxScene<해당 함수는 sceneside_current값을 쓸 수 없습니다>", Side != sceneside_current);
		BxASSERT("BxScene<해당 함수는 scenelayer_current값을 쓸 수 없습니다>", Layer != scenelayer_current);
		BxASSERT("BxScene<해당 이름의 씬은 존재하지 않습니다>", __FindAllScene__(Name) != nullptr);
		AllScene* FindedScene = __FindAllScene__(Name);
		// New Request(Add)
		ActiveSceneRequest* NewActiveSceneRequest = BxNew_Param(ActiveSceneRequest, sysrequest_add,
			Layer, Message, BxNew_Param(ActiveScene, FindedScene, Side));
		NewActiveSceneRequest->Next = __GetActiveSceneRequest__().Next;
		__GetActiveSceneRequest__().Next = NewActiveSceneRequest;
		++__GetActiveSceneRequestCount__();
	}

	/*!
	\brief 활성씬 제거
	\param Name : 씬이름
	\param Side : 퇴장할 방향
	\see AddRequest
	*/
	global_func void SubRequest(string Name, sceneside Side = sceneside_current)
	{
		BxASSERT("BxScene<해당 이름의 씬은 존재하지 않습니다>", __FindAllScene__(Name) != nullptr);
		AllScene* FindedScene = __FindActiveScene__(Name);
		// Find Loop
		ActiveScene* OneActiveScene = &__GetActiveScene__();
		while(OneActiveScene->Scene != FindedScene)
			OneActiveScene = OneActiveScene->Next;
		// New Request(Sub)
		if(Side != sceneside_current) OneActiveScene->SetSide(Side);
		ActiveSceneRequest* NewActiveSceneRequest = BxNew_Param(ActiveSceneRequest, sysrequest_sub,
			scenelayer_current, "", OneActiveScene);
		NewActiveSceneRequest->Next = __GetActiveSceneRequest__().Next;
		__GetActiveSceneRequest__().Next = NewActiveSceneRequest;
		++__GetActiveSceneRequestCount__();
	}

	/*!
	\brief 활성씬 이동설정
	\param Name : 씬이름
	\param Side : 이동할 방향
	\param Layer : 변경할 레이어
	*/
	global_func void SetRequest(string Name, sceneside Side = sceneside_current, scenelayer Layer = scenelayer_current)
	{
		BxASSERT("BxScene<해당 함수는 scenelayer_gui값을 쓸 수 없습니다>", Layer != scenelayer_gui);
		BxASSERT("BxScene<해당 이름의 씬은 존재하지 않습니다>", __FindAllScene__(Name) != nullptr);
		AllScene* FindedScene = __FindActiveScene__(Name);
		// Find Loop
		ActiveScene* OneActiveScene = &__GetActiveScene__();
		while(OneActiveScene->Scene != FindedScene)
			OneActiveScene = OneActiveScene->Next;
		// New Request(Set)
		if(Side == sceneside_current || OneActiveScene->SetSide(Side))
		{
			ActiveSceneRequest* NewActiveSceneRequest = BxNew_Param(ActiveSceneRequest, sysrequest_set,
				Layer, "", OneActiveScene);
			NewActiveSceneRequest->Next = __GetActiveSceneRequest__().Next;
			__GetActiveSceneRequest__().Next = NewActiveSceneRequest;
			++__GetActiveSceneRequestCount__();
		}
	}

	typedef syseventresult (*EventCB)(void* Data, const sysevent& Event);
	typedef sysupdateresult (*UpdateCB)(void* Data);
	typedef void (*RenderCB)(void* Data, BxDraw& Draw);
	typedef void* (*GetDataCB)(sysmethod Mode);

	/// @cond SECTION_NAME
	class AllScene
	{
	public:
		const BxString::Parse Name;
		const EventCB Event;
		const UpdateCB Update;
		const RenderCB Render;
		const GetDataCB GetData;
		BxString::Parse BuildDate;
		AllScene* Next;
	public:
		AllScene(string name, EventCB event, UpdateCB update, RenderCB render, GetDataCB getdata, string buildday, string buildtime)
			: Name(name), Event(event), Update(update), Render(render), GetData(getdata), Next(nullptr)
		{
			BuildDate = buildday;
			BuildDate += " ";
			BuildDate += buildtime;
		}
		~AllScene() {BxDelete(Next);}
	};
	/// @endcond

	/// @cond SECTION_NAME
	class ActiveScene
	{
	public:
		global_func sceneflow& CurrentFlow()
		{global_data sceneflow _ = sceneflow_1x; return _;}
	public:
		AllScene* _ref_ Scene;
		bool EnableKey;
		bool EnableTouch;
		sceneside BeforeSide;
		sceneside CurrentSide;
		huge SideStep;
		fint SideValue;
		bool IsSubRequest;
		ActiveScene* Next;
	public:
		ActiveScene(AllScene* _ref_ layer, sceneside side) : Scene(layer)
		{
			EnableKey = false;
			EnableTouch = false;
			BeforeSide = side;
			CurrentSide = sceneside_center;
			SideStep = 0;
			SideValue = (BeforeSide == CurrentSide)? 0x10000 : 0;
			IsSubRequest = false;
			Next = nullptr;
		}
		~ActiveScene() {BxDelete(Next);}
	public:
		bool SetSide(sceneside side)
		{
			if(SideValue < 0x10000)
				return false;
			BeforeSide = CurrentSide;
			CurrentSide = side;
			SideStep = 0;
			if(BeforeSide != CurrentSide)
				SideValue = 0;
			return true;
		}
		bool IsFlowing()
		{return (SideValue < 0x10000 || IsSubRequest);}
		void ToFlow(int& x, int& y)
		{
			if(BeforeSide != sceneside_center || CurrentSide != sceneside_center)
			{
				const int Width = BxCore::Surface::GetWidthHW();
				const int Height = BxCore::Surface::GetHeightHW();
				int XB = 0, YB = 0;
				switch(BeforeSide)
				{
				case sceneside_up: YB = -Height; break;
				case sceneside_down: YB = Height; break;
				case sceneside_left: XB = -Width; break;
				case sceneside_right: XB = Width; break;
				}
				int XE = 0, YE = 0;
				switch(CurrentSide)
				{
				case sceneside_up: YE = -Height; break;
				case sceneside_down: YE = Height; break;
				case sceneside_left: XE = -Width; break;
				case sceneside_right: XE = Width; break;
				}
				const int AddX = FtoR(XB * (0x10000 - SideValue) + XE * SideValue);
				const int AddY = FtoR(YB * (0x10000 - SideValue) + YE * SideValue);
				x += AddX;
				y += AddY;
			}
		}
		void DoFlow()
		{
			if(SideValue < 0x10000)
			{
				const int FrameTime = BxCore::Main::GetRealFrameTime();
				SideStep = BxUtilGlobal::Min(0x10000, SideStep + 0x10000 * FrameTime / 1000);
				switch(CurrentFlow())
				{
				case sceneflow_1x: SideValue = (fint) SideStep; break;
				case sceneflow_2x: SideValue = (fint) BxUtilGlobal::Min(0x10000, SideStep * 2); break;
				case sceneflow_4x: SideValue = (fint) BxUtilGlobal::Min(0x10000, SideStep * 4); break;
				case sceneflow_accel: SideValue = (fint) (SideStep * SideStep / 0x10000); break;
				case sceneflow_decel:
					SideValue = (fint) (0x10000 - (0x10000 - SideStep) * (0x10000 - SideStep) / 0x10000);
					break;
				}
			}
			else if(IsSubRequest)
			{
				// Scene Quit
				sysevent Event;
				BxCore::Util::MemSet(&Event, 0, sizeof(sysevent));
				Event.type = syseventtype_quit;
				Scene->Event(Scene->GetData(sysmethod_get), Event);
				// Remove SceneData
				Scene->GetData(sysmethod_remove);
				// Sub ActiveScene
				__SubActiveScene__(this);
			}
		}
	};
	/// @endcond

	/// @cond SECTION_NAME
	class ActiveSceneRequest
	{
	public:
		const sysrequest Type;
		const scenelayer Layer;
		const BxString::Parse Message;
		ActiveScene* _ref_ Data;
		ActiveSceneRequest* Next;
	public:
		ActiveSceneRequest(sysrequest type, scenelayer layer, string message, ActiveScene* _ref_ data)
			: Type(type), Layer(layer), Message((message)? message : ""), Data(data), Next(nullptr) {}
		~ActiveSceneRequest() {BxDelete(Next);}
	};
	/// @endcond

	/// @cond SECTION_NAME
	class EventQueue
	{
	public:
		sysevent Event;
		syseventset Set;
		EventQueue* Next;
		EventQueue() : Next(nullptr) {}
		~EventQueue() {delete Next;}
		global_func EventQueue* Top(bool doflip = false)
		{
			global_data EventQueue _Top[2];
			global_data int Focusing = 0;
			EventQueue* Result = &_Top[Focusing];
			if(doflip) Focusing = 1 - Focusing;
			return Result;
		}
	};
	/// @endcond

	/*!
	\brief 활성씬 수량얻기
	\return 수량
	*/
	global_func int GetCount()
	{
		return __GetActiveSceneCount__() - __GetActiveSceneGUICount__();
	}

	/*!
	\brief 해당방향의 최상위 활성씬 이름얻기
	\param Side : 방향
	\return 씬이름
	*/
	global_func string GetName(sceneside Side)
	{
		BxASSERT("BxScene<해당 함수는 sceneside_current값을 쓸 수 없습니다>", Side != sceneside_current);
		int LayerID = __GetActiveSceneCount__() - 1;
		ActiveScene* OneActiveScene = &__GetActiveScene__();
		while((OneActiveScene = OneActiveScene->Next) != nullptr)
			if(LayerID-- < GetCount() && OneActiveScene->CurrentSide == Side)
				return OneActiveScene->Scene->Name;
		return nullptr;
	}

	/*!
	\brief 해당레이어의 활성씬 이름얻기
	\param Layer : 레이어
	\return 씬이름
	*/
	global_func string GetName(scenelayer Layer)
	{
		BxASSERT("BxScene<해당 함수는 scenelayer_current값을 쓸 수 없습니다>", Layer != scenelayer_current);
		BxASSERT("BxScene<검색할 씬의 순번이 너무 큽니다>", (int) Layer < GetCount());
		if(Layer == scenelayer_gui) Layer = (scenelayer) (__GetActiveSceneCount__() - 1);
		else if(Layer == scenelayer_top) Layer = (scenelayer) (GetCount() - 1);
		// 탐색루프
		int LayerID = __GetActiveSceneCount__() - 1;
		ActiveScene* OneActiveScene = &__GetActiveScene__();
		while((OneActiveScene = OneActiveScene->Next) != nullptr)
			if(LayerID-- == (int) Layer)
				return OneActiveScene->Scene->Name;
		return nullptr;
	}

	/*!
	\brief 활성씬 방향얻기
	\param Name : 씬이름
	\return 방향
	*/
	global_func sceneside GetSide(string Name)
	{
		BxASSERT("BxScene<해당 이름의 씬은 존재하지 않습니다>", __FindAllScene__(Name) != nullptr);
		ActiveScene* OneActiveScene = &__GetActiveScene__();
		while((OneActiveScene = OneActiveScene->Next) != nullptr)
			if(BxUtilGlobal::StrCmp(OneActiveScene->Scene->Name, Name) == same)
				return OneActiveScene->CurrentSide;
		return sceneside_center;
	}

	/*!
	\brief 활성씬 레이어얻기
	\param Name : 씬이름
	\return 레이어
	*/
	global_func scenelayer GetLayer(string Name)
	{
		BxASSERT("BxScene<해당 이름의 씬은 존재하지 않습니다>", __FindAllScene__(Name) != nullptr);
		int LayerID = __GetActiveSceneCount__() - 1;
		ActiveScene* OneActiveScene = &__GetActiveScene__();
		while((OneActiveScene = OneActiveScene->Next) != nullptr)
		{
			if(BxUtilGlobal::StrCmp(OneActiveScene->Scene->Name, Name) == same)
				return (scenelayer) LayerID;
			--LayerID;
		}
		return scenelayer_1st;
	}

	/*!
	\brief 씬이동의 방식설정
	\param Flow : 이동방식
	*/
	global_func void SetFlow(sceneflow Flow)
	{
		ActiveScene::CurrentFlow() = Flow;
	}

	/*!
	\brief 해당 씬의 활성화 상태얻기
	\param Name : 씬이름
	\return 활성화여부
	*/
	global_func bool IsActivity(string Name)
	{
		BxASSERT("BxScene<해당 이름의 씬은 존재하지 않습니다>", __FindAllScene__(Name) != nullptr);
		AllScene* FindedScene = __FindActiveScene__(Name);
		return (FindedScene != nullptr);
	}

	/*!
	\brief 활성씬의 데이터얻기
	\param Name : 씬이름
	\return 데이터
	*/
	global_func void* GetData(string Name)
	{
		BxASSERT("BxScene<해당 이름의 씬은 존재하지 않습니다>", __FindAllScene__(Name) != nullptr);
		AllScene* FindedScene = __FindActiveScene__(Name);
		if(FindedScene == nullptr) return nullptr;
		return FindedScene->GetData(sysmethod_get);
	}

	/*!
	\brief 활성씬의 위치얻기
	\param Name : 씬이름
	\return 좌상좌표
	*/
	global_func point GetPosition(string Name)
	{
		BxASSERT("BxScene<해당 이름의 씬은 존재하지 않습니다>", __FindAllScene__(Name) != nullptr);
		ActiveScene* OneActiveScene = &__GetActiveScene__();
		while((OneActiveScene = OneActiveScene->Next) != nullptr)
			if(BxUtilGlobal::StrCmp(OneActiveScene->Scene->Name, Name) == same)
			{
				int FlowedX = 0, FlowedY = 0;
				OneActiveScene->ToFlow(FlowedX, FlowedY);
				return XY(FlowedX, FlowedY);
			}
		return point::zero();
	}

	/// @cond SECTION_NAME
	global_func bool __Create__(string Name, EventCB Event, UpdateCB Update, RenderCB Render, GetDataCB GetData, string BuildDay, string BuildTime)
	{
		BxASSERT("BxScene<같은 씬이름을 가진 소스코드가 이미 존재합니다>", __FindAllScene__(Name) == nullptr);
		AllScene* NewScene = BxNew_Param(AllScene, Name, Event, Update, Render, GetData, BuildDay, BuildTime);
		NewScene->Next = __GetAllScene__().Next;
		__GetAllScene__().Next = NewScene;
		++__GetAllSceneCount__();
		return true;
	}
	/// @endcond

	/// @cond SECTION_NAME
	global_func void __AddEvent__(sysevent& Event, syseventset Set)
	{
		EventQueue* NewEvent = new EventQueue();
		BxCore::Util::MemCpy(&NewEvent->Event, &Event, sizeof(sysevent));
		BxCore::Util::MemCpy(&NewEvent->Set, &Set, sizeof(syseventset));
		EventQueue* CurNode = EventQueue::Top();
		while(CurNode->Next) CurNode = CurNode->Next;
		CurNode->Next = NewEvent;
	}

	/// @cond SECTION_NAME
	global_func bool __OnEvent__(BxDraw& Draw)
	{
		bool DoEvent = false;
		EventQueue* TopNode = EventQueue::Top(true);
		EventQueue* CurNode = TopNode;
		while(CurNode = CurNode->Next)
		{
			DoEvent = true;
			sysevent& Event = CurNode->Event;
			syseventset Set = CurNode->Set;
			bool NeedRecall = true;
			while(NeedRecall)
			{
				NeedRecall = false;
				switch(Event.type)
				{
				case syseventtype_touch:
					//if(Event.touch.id == 0)
					{
						int SelectedButton = -1;
						if(Event.touch.type == systouchtype_down)
						{
							for(int i = Draw.ButtonLength - 1; 0 <= i; --i)
							{
								if(Event.touch.x < Draw.ButtonEvent[i].Field.l || Event.touch.y < Draw.ButtonEvent[i].Field.t ||
									Draw.ButtonEvent[i].Field.r <= Event.touch.x || Draw.ButtonEvent[i].Field.b <= Event.touch.y)
									continue;
								SelectedButton = i;
								Draw.CapturedButtonEvent.SetEvent(Draw.ButtonEvent[i].Name, Draw.ButtonEvent[i].Field);
								break;
							}
						}
						else if(*Draw.CapturedButtonEvent.Name)
						{
							for(int i = Draw.ButtonLength - 1; 0 <= i; --i)
							{
								if(BxUtilGlobal::StrCmp(Draw.CapturedButtonEvent.Name, Draw.ButtonEvent[i].Name) != same)
									continue;
								SelectedButton = i;
								break;
							}
							if(Event.touch.type != systouchtype_move)
							{
								Draw.CapturedButtonEvent.DeleteEvent();
								NeedRecall = true;
							}
						}
						if(-1 < SelectedButton)
						{
							Event.type = syseventtype_button;
							switch(Event.touch.type)
							{
							case systouchtype_down: Event.button.type = sysbuttontype_down; break;
							case systouchtype_move: Event.button.type = sysbuttontype_drag; break;
							case systouchtype_up:
							case systouchtype_cancel: Event.button.type = sysbuttontype_up; break;
							}
							Event.button.name = Draw.ButtonEvent[SelectedButton].Name;
							Event.button.field = Draw.ButtonEvent[SelectedButton].Field;
							Event.button.x = Event.touch.x - Draw.ButtonEvent[SelectedButton].Field.l;
							Event.button.y = Event.touch.y - Draw.ButtonEvent[SelectedButton].Field.t;
						}
					}
					break;
				case syseventtype_button:
					if(Event.button.type == sysbuttontype_up)
					{
						for(int i = Draw.ButtonLength - 1; 0 <= i; --i)
						{
							if(Event.touch.x < Draw.ButtonEvent[i].Field.l || Event.touch.y < Draw.ButtonEvent[i].Field.t ||
								Draw.ButtonEvent[i].Field.r <= Event.touch.x || Draw.ButtonEvent[i].Field.b <= Event.touch.y)
								continue;
							if(BxUtilGlobal::StrCmp(Event.button.name, Draw.ButtonEvent[i].Name) != same)
							{
								Event.button.type = sysbuttontype_drop;
								Event.button.name = Draw.ButtonEvent[i].Name;
								Event.button.field = Draw.ButtonEvent[i].Field;
								Event.button.x = Event.touch.x - Draw.ButtonEvent[i].Field.l;
								Event.button.y = Event.touch.y - Draw.ButtonEvent[i].Field.t;
							}
							break;
						}
						if(Event.button.type != sysbuttontype_drop)
						{
							NeedRecall = false;
							continue;
						}
					}
					break;
				}
				// 이벤트처리
				ActiveScene* OneActiveScene = &__GetActiveScene__();
				bool IsEventDone = (Event.type == syseventtype_touch && Event.touch.id == 0
					&& BxPanel::CalcTouch(Event.touch.type, Event.touch.x, Event.touch.y));
				while(!IsEventDone && (OneActiveScene = OneActiveScene->Next) != nullptr)
				{
					if(OneActiveScene->IsFlowing())
						continue;
					int OldTouchX = 0, OldTouchY = 0, FlowedX = 0, FlowedY = 0;
					switch(Event.type)
					{
					case syseventtype_init: break;
					case syseventtype_quit: break;
					case syseventtype_resize: break;
					case syseventtype_key:
						if(Set == syseventset_need_enable && OneActiveScene->EnableKey == false) continue;
						if(Set == syseventset_do_enable) OneActiveScene->EnableKey = true;
						break;
					case syseventtype_touch:
						OldTouchX = Event.touch.x;
						OldTouchY = Event.touch.y;
						OneActiveScene->ToFlow(FlowedX, FlowedY);
						Event.touch.x -= FlowedX;
						Event.touch.y -= FlowedY;
					case syseventtype_button:
						if(Set == syseventset_need_enable && OneActiveScene->EnableTouch == false) continue;
						if(Set == syseventset_do_enable) OneActiveScene->EnableTouch = true;
						break;
					}
					if(OneActiveScene->Scene->Event(OneActiveScene->Scene->GetData(sysmethod_get), Event) == syseventresult_done)
					{
						BxASSERT("BxScene<해당 이벤트는 독점할 수 없습니다>", Event.type != syseventtype_init);
						BxASSERT("BxScene<해당 이벤트는 독점할 수 없습니다>", Event.type != syseventtype_quit);
						BxASSERT("BxScene<해당 이벤트는 독점할 수 없습니다>", Event.type != syseventtype_resize);
						IsEventDone = true;
					}
					if(Event.type == syseventtype_touch)
					{
						Event.touch.x = OldTouchX;
						Event.touch.y = OldTouchY;
					}
				}
				// 이벤트완료시 하위처리
				if(IsEventDone)
				while((OneActiveScene = OneActiveScene->Next) != nullptr)
				{
					switch(Event.type)
					{
					case syseventtype_init: break;
					case syseventtype_quit: break;
					case syseventtype_resize: break;
					case syseventtype_key:
						OneActiveScene->EnableKey = false;
						break;
					case syseventtype_touch:
					case syseventtype_button:
						OneActiveScene->EnableTouch = false;
						break;
					}
				}
				__UpdateActiveScene__();
			}
		}
		delete TopNode->Next;
		TopNode->Next = nullptr;
		return DoEvent;
	}
	/// @endcond

	/// @cond SECTION_NAME
	global_func bool __OnEventUpdateRender__(BxDraw& Draw)
	{
		// Event
		sysupdateresult UpdateResult = (BxScene::__OnEvent__(Draw))? sysupdateresult_do_render : sysupdateresult_no_render_fixed_frame;
		// Update
		ActiveScene* OneActiveScene = &__GetActiveScene__();
		while((OneActiveScene = OneActiveScene->Next) != nullptr)
		{
			if(OneActiveScene->IsFlowing())
			{
				UpdateResult = (sysupdateresult) (UpdateResult | sysupdateresult_do_render_fixed_frame);
				continue;
			}
			UpdateResult = (sysupdateresult) (UpdateResult | OneActiveScene->Scene->Update(OneActiveScene->Scene->GetData(sysmethod_get)));
		}
		// Render
		const bool DoRender = ((UpdateResult & sysupdateresult_do_render_fixed_frame) == sysupdateresult_do_render_fixed_frame);
		if(DoRender)
		{
			BxPanel::PaintReady(Draw);
			if((UpdateResult & sysupdateresult_do_render) == sysupdateresult_do_render)
				BxCore::Main::PullFrameDelay();
			const int ActiveSceneCount = __GetActiveSceneCount__();
			for(int i = ActiveSceneCount; 0 < i; --i)
			{
				// 역순접근
				ActiveScene* OneActiveScene = &__GetActiveScene__();
				for(int j = 0; j < i; ++j) OneActiveScene = OneActiveScene->Next;
				// 마진모드 설정
				const bool IsGeneralScene = (__GetActiveSceneGUICount__() < i);
				const bool OldMarginMode = BxCore::Surface::SetMarginMode(IsGeneralScene);
				// 클리핑 스타일링 설정
				int FlowedX = (IsGeneralScene)? BxCore::Main::GetGUIMarginL() : 0;
				int FlowedY = (IsGeneralScene)? BxCore::Main::GetGUIMarginT() : 0;
				OneActiveScene->ToFlow(FlowedX, FlowedY);
				Draw.AddStyle(CLIP(XYWH(FlowedX, FlowedY, BxCore::Surface::GetWidthHW(), BxCore::Surface::GetHeightHW())));
				// 랜더링호출
				OneActiveScene->Scene->Render(OneActiveScene->Scene->GetData(sysmethod_get), Draw);
				// 전체 스타일링 해제
				Draw.SubStyle(true);
				// 마진모드 해제
				BxCore::Surface::SetMarginMode(OldMarginMode);
			}
			// 씬이동
			ActiveScene* OneActiveScene = &__GetActiveScene__();
			ActiveScene* NextActiveScene = OneActiveScene->Next;
			while((OneActiveScene = NextActiveScene) != nullptr)
			{
				NextActiveScene = OneActiveScene->Next;
				OneActiveScene->DoFlow();
			}
		}
		else if((UpdateResult & sysupdateresult_no_render) == sysupdateresult_no_render)
			BxCore::Main::PushFrameDelay();
		__UpdateActiveScene__();
		return DoRender;
	}
	/// @endcond

	/// @cond SECTION_NAME
	global_func void __UpdateActiveScene__()
	{
		// 요청정리
		const int ActiveSceneRequestCount = __GetActiveSceneRequestCount__();
		__GetActiveSceneRequestCount__() = 0;
		ActiveSceneRequest* OldActiveSceneRequest = __GetActiveSceneRequest__().Next;
		__GetActiveSceneRequest__().Next = nullptr;
		// 요청실행
		sysevent Event;
		BxCore::Util::MemSet(&Event, 0, sizeof(sysevent));
		for(int i = ActiveSceneRequestCount - 1; 0 <= i; --i)
		{
			ActiveSceneRequest* OneActiveSceneRequest = OldActiveSceneRequest;
			for(int j = 0; j < i; ++j) OneActiveSceneRequest = OneActiveSceneRequest->Next;
			const bool IsExist = __IsExistActiveScene__(OneActiveSceneRequest->Data->Scene);
			switch(OneActiveSceneRequest->Type)
			{
			case sysrequest_null: break;
			case sysrequest_add:
				if(!IsExist)
				{
					ActiveScene* NewActiveScene = OneActiveSceneRequest->Data;
					const int LayerID = (OneActiveSceneRequest->Layer < scenelayer_1st)?
						((OneActiveSceneRequest->Layer == scenelayer_gui)? 0 : __GetActiveSceneGUICount__()) :
						__GetActiveSceneCount__() - (int) OneActiveSceneRequest->Layer;
					BxASSERT("BxScene<추가할 씬의 순번이 너무 큽니다>", 0 <= LayerID);
					ActiveScene* PrevNewActiveScene = &__GetActiveScene__();
					for(int i = 0; i < LayerID; ++i)
						PrevNewActiveScene = PrevNewActiveScene->Next;
					NewActiveScene->Next = PrevNewActiveScene->Next;
					PrevNewActiveScene->Next = NewActiveScene;
					++__GetActiveSceneCount__();
					if(OneActiveSceneRequest->Layer == scenelayer_gui)
						++__GetActiveSceneGUICount__();
					// Scene Init(SceneData Create)
					Event.type = syseventtype_init;
					Event.init.w = BxCore::Surface::GetWidth();
					Event.init.h = BxCore::Surface::GetHeight();
					Event.init.message = OneActiveSceneRequest->Message;
					void* Data = OneActiveSceneRequest->Data->Scene->GetData(sysmethod_get);
					OneActiveSceneRequest->Data->Scene->Event(Data, Event);
				}
				else __SubActiveScene__(OneActiveSceneRequest->Data);
				break;
			case sysrequest_sub:
				OneActiveSceneRequest->Data->IsSubRequest = true;
				break;
			case sysrequest_set:
				if(IsExist && OneActiveSceneRequest->Layer != scenelayer_current)
				{
					ActiveScene* CurActiveScene = OneActiveSceneRequest->Data;
					const int LayerID = (OneActiveSceneRequest->Layer == scenelayer_top)?
						__GetActiveSceneGUICount__() : __GetActiveSceneCount__() - (int) OneActiveSceneRequest->Layer;
					BxASSERT("BxScene<추가할 씬의 순번이 너무 큽니다>", 0 <= LayerID);
					ActiveScene* PrevSetActiveScene = &__GetActiveScene__();
					for(int i = 0; i < LayerID; ++i)
						PrevSetActiveScene = PrevSetActiveScene->Next;
					if(PrevSetActiveScene != CurActiveScene && PrevSetActiveScene->Next != CurActiveScene)
					{
						ActiveScene* PrevCurActiveScene = &__GetActiveScene__();
						while(PrevCurActiveScene->Next != CurActiveScene)
							PrevCurActiveScene = PrevCurActiveScene->Next;
						PrevCurActiveScene->Next = CurActiveScene->Next;
						CurActiveScene->Next = PrevSetActiveScene->Next;
						PrevSetActiveScene->Next = CurActiveScene;
					}
				}
				break;
			}
		}
		BxDelete(OldActiveSceneRequest);
	}
	/// @endcond

	/// @cond SECTION_NAME
	global_func AllScene* __FindAllScene__(string Name)
	{
		AllScene* FindedScene = &__GetAllScene__();
		while((FindedScene = FindedScene->Next) != nullptr)
			if(BxUtilGlobal::StrCmp(FindedScene->Name, Name) == same)
				return FindedScene;
		return nullptr;
	}
	/// @endcond

	/// @cond SECTION_NAME
	global_func AllScene* __FindActiveScene__(string Name)
	{
		ActiveScene* FindedActiveScene = &__GetActiveScene__();
		while((FindedActiveScene = FindedActiveScene->Next) != nullptr)
			if(BxUtilGlobal::StrCmp(FindedActiveScene->Scene->Name, Name) == same)
				return FindedActiveScene->Scene;
		return nullptr;
	}
	/// @endcond

	/// @cond SECTION_NAME
	global_func bool __IsExistActiveScene__(AllScene* Scene)
	{
		ActiveScene* FindedActiveScene = &__GetActiveScene__();
		while((FindedActiveScene = FindedActiveScene->Next) != nullptr)
			if(FindedActiveScene->Scene == Scene) return true;
		return false;
	}
	/// @endcond

	/// @cond SECTION_NAME
	global_func void __SubActiveScene__(ActiveScene* Scene)
	{
		ActiveScene* PrevDeleteActiveScene = &__GetActiveScene__();
		int LayerID = 0;
		while(PrevDeleteActiveScene->Next != Scene)
		{
			++LayerID;
			PrevDeleteActiveScene = PrevDeleteActiveScene->Next;
			if(!PrevDeleteActiveScene)
			{
				BxDelete(Scene);
				return;
			}
		}
		ActiveScene* DeleteActiveScene = Scene;
		PrevDeleteActiveScene->Next = DeleteActiveScene->Next;
		DeleteActiveScene->Next = nullptr;
		BxDelete(DeleteActiveScene);
		--__GetActiveSceneCount__();
		if(LayerID < __GetActiveSceneGUICount__())
			--__GetActiveSceneGUICount__();
	}
	/// @endcond

	/// @cond SECTION_NAME
	global_func ActiveScene* __FindActiveSceneRequest__(AllScene* Scene)
	{
		ActiveSceneRequest* FindedActiveSceneRequest = &__GetActiveSceneRequest__();
		while((FindedActiveSceneRequest = FindedActiveSceneRequest->Next) != nullptr)
			if(FindedActiveSceneRequest->Data->Scene == Scene)
				return FindedActiveSceneRequest->Data;
		return nullptr;
	}
	/// @endcond

	/// @cond SECTION_NAME
	// Scene
	global_func inline AllScene& __GetAllScene__() {global_data AllScene Begin("", nullptr, nullptr, nullptr, nullptr, "", ""); return Begin;}
	global_func inline int& __GetAllSceneCount__() {global_data int _ = 0; return _;}
	global_func inline ActiveScene& __GetActiveScene__() {global_data ActiveScene Begin(nullptr, sceneside_center); return Begin;}
	global_func inline int& __GetActiveSceneCount__() {global_data int _ = 0; return _;}
	global_func inline int& __GetActiveSceneGUICount__() {global_data int _ = 0; return _;}
	// Request
	global_func inline ActiveSceneRequest& __GetActiveSceneRequest__()
	{global_data ActiveSceneRequest Begin(sysrequest_null, scenelayer_current, "", nullptr); return Begin;}
	global_func inline int& __GetActiveSceneRequestCount__() {global_data int RequestCount = 0; return RequestCount;}
	/// @endcond
};

#define FRAMEWORK_SCENE(DATA, STRING) \
	local_func macro syseventresult OnEvent(DATA& Data, const sysevent& Event); \
	local_func macro sysupdateresult OnUpdate(DATA& Data); \
	local_func macro void OnRender(DATA& Data, BxDraw& Draw); \
	local_func syseventresult __OnEvent__(void* Data, const sysevent& Event) {return OnEvent(*((DATA*) Data), Event);} \
	local_func sysupdateresult __OnUpdate__(void* Data) {return OnUpdate(*((DATA*) Data));} \
	local_func void __OnRender__(void* Data, BxDraw& Draw) {OnRender(*((DATA*) Data), Draw);} \
	local_func void* __GetData__(sysmethod Mode) \
	{ \
		global_data DATA* Data = nullptr; \
		if(Mode == sysmethod_remove) \
		{ \
			BxDelete(Data); \
			Data = nullptr; \
		} \
		else if(!Data) \
			Data = BxNew(DATA); \
		return (void*) Data; \
	} \
	local_data bool _Unknown = BxScene::__Create__(STRING, __OnEvent__, __OnUpdate__, __OnRender__, __GetData__, __DATE__, __TIME__);
