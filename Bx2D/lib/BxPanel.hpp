#pragma once
#include <BxDraw.hpp>
#include <BxVarMap.hpp>
#include <BxVarVector.hpp>
#include <BxImage.hpp>
#include <BxString.hpp>
using namespace BxUtilGlobal;
using namespace BxDrawGlobal;

class BxPanel
{
	friend class BxScene;

	////////////////////////////////////////////////////////////
	// ■ Enumerations
	////////////////////////////////////////////////////////////
	public: enum {CellSize = 128};
	public: enum {ScrollSensitive = 256};
	public: enum Param {prmXY, prmXYW, prmXYXY, prmXYWH};
	public: enum Touch {tchDown, tchMoveIn, tchMoveOut, tchUpIn, tchUpOut, tchUpAuto, tchDrag, tchDrop};
	public: enum View {viewUnknown, viewNormal, viewScroll, viewMax};
	public: typedef void (*EventCB)(BxPanel*, string, void*);
	private: typedef void (*CreateCB)(string, unknown);
	private: typedef unknown (*NotifyCB)(string, unknown);
	private: typedef string (*TouchCB)(Touch);
	private: typedef void (*PaintCB)(Param, const BxArgument&, rect&);
	private: typedef BxPanel* (*AllocCB)(BxPanel*);
	private: typedef void (*FreeCB)(BxPanel**);
	private: typedef void** (*ServiceCB)(void*);

	////////////////////////////////////////////////////////////
	// ■ Funtions
	////////////////////////////////////////////////////////////
	public: template<typename TYPE = unknown>
	void Bind(string type, string option = "", TYPE param = nullptr)
	{
		if(VarL->RefFunc) VarL->RefFunc->Free(&VarL->VarM);
		VarL->RefFunc = Function::Get(type, false);
        BxASSERT(BxString("<>:BxPanel<<해당 패널(<A>)이 존재하지 않습니다>>", BxARG(type)),
            VarL->RefFunc != nullptr);
		VarL->VarM = VarL->RefFunc->Alloc(this);
		void** VarS = VarL->RefFunc->Service(VarL->VarM);
		if(!*VarS) *VarS = new VariableS();
		VarL->RefFunc->Create(option, (unknown) param);
	}
	public: template<typename TYPE1 = unknown, typename TYPE2 = unknown>
	TYPE1 Notify(string message, TYPE2 param = nullptr)
	{
		VarL->RefFunc->Service(VarL->VarM);
		return (TYPE1) VarL->RefFunc->Notify(message, (unknown) param);
	}
	public: const point Paint(Param type, const BxArgument& args)
	{
		rect Result = BxDrawGlobal::_DrawOption::CurClipRect();
		Result.r -= Result.l;
		Result.b -= Result.t;
		VarL->RefFunc->Service(VarL->VarM);
		VarL->LastType = viewUnknown;
		VarL->RefFunc->Paint(type, args, Result);
		// 패널디버그
		if(BxCore::System::IsEnablePanelDebug())
		{
			if(VarL->LastType != viewUnknown)
			{
				color_x888 LineColor[viewMax] = {RGB32(0, 0, 0), RGB32(255, 0, 0), RGB32(0, 0, 255)};
				BxDraw& Draw = GetDraw();
				BxTRY(Draw, COLOR(LineColor[VarL->LastType]) >> HOTSPOT(
					BxCore::Main::GetGUIMarginL() - BxDrawGlobal::_DrawOption::CurHotspotX(),
					BxCore::Main::GetGUIMarginT() - BxDrawGlobal::_DrawOption::CurHotspotY()))
				{
					const byte OpacityValue = 32 + 8 * VarL->DebugLineHit;
					const byte OpacityValueBold = 255 - (255 - OpacityValue) / 2;
					const int Border = (VarL->LastType == viewScroll)? 20 : 5;
					if(VarL->ClickedData.Callbacks.Length() == 0)
					{
						BxTRY(Draw, OPACITY(OpacityValue))
						{
							Draw.Rectangle(FILL, XYXY(VarL->LastRect.l, VarL->LastRect.t, VarL->LastRect.r, VarL->LastRect.t + Border));
							Draw.Rectangle(FILL, XYXY(VarL->LastRect.l, VarL->LastRect.b - Border, VarL->LastRect.r, VarL->LastRect.b));
							Draw.Rectangle(FILL, XYXY(VarL->LastRect.l, VarL->LastRect.t + Border, VarL->LastRect.l + Border, VarL->LastRect.b - Border));
							Draw.Rectangle(FILL, XYXY(VarL->LastRect.r - Border, VarL->LastRect.t + Border, VarL->LastRect.r, VarL->LastRect.b - Border));
						}
						if(0 < VarL->DebugLineHit)
							Draw.Rectangle(FILL, XYXY(VarL->LastRect.l + Border, VarL->LastRect.t + Border, VarL->LastRect.r - Border, VarL->LastRect.b - Border),
								COLOR(255, 255, 255, 1000) >> OPACITY(OpacityValueBold));
					}
					else Draw.Rectangle(FILL, XYXY(VarL->LastRect.l, VarL->LastRect.t, VarL->LastRect.r, VarL->LastRect.b), OPACITY(OpacityValue));
					// 마감강조(┛)
					Draw.Rectangle(FILL, XYXY(VarL->LastRect.l, VarL->LastRect.b - 1, VarL->LastRect.r, VarL->LastRect.b), OPACITY(OpacityValue));
					Draw.Rectangle(FILL, XYXY(VarL->LastRect.r - 1, VarL->LastRect.t, VarL->LastRect.r, VarL->LastRect.b - 1), OPACITY(OpacityValue));
					// 패널명
					if(0 < VarL->DebugLineHit)
					{
						const int Gap = 3;
						const int MinX = BxCore::Surface::GetWidthHW() - BxCore::Font::GetWidth(BxCore::Font::Default(), GetTypeText()) - Gap;
						BxTRY(Draw, CLIP(XYWH(BxUtilGlobal::Min(VarL->LastRect.l + Border + Gap, MinX), VarL->LastRect.t + Border + Gap)))
						{
							BxTRY(Draw, COLOR(0, 0, 0, 1000) >> OPACITY(OpacityValueBold))
							{
								Draw.Text(BxCore::Font::Default(), GetTypeText(), XY(1, 1));
								Draw.Text(BxCore::Font::Default(), GetTypeText(), XY(1, 0));
							}
							Draw.Text(BxCore::Font::Default(), GetTypeText(), XY(0, 0));
						}
					}
				}
			}
			if(0 < VarL->DebugLineHit)
				--VarL->DebugLineHit;
		}
		return XY(Result.r, Result.b);
	}
	public: bool IsBinded() {return (VarL->RefFunc != nullptr);}
	public: id_text GetTypeText() {return (VarL->RefFunc)? VarL->RefFunc->TypeText : nullptr;}
	public: bool GetEnable() {return VarL->Enable;}
	public: void SetEnable(bool enable) {VarL->Enable = enable;}
	public: bool GetVisible() {return VarL->Visible;}
	public: void SetVisible(bool visible) {VarL->Visible = visible;}
	public: void* GetUserData(int index) {return VarL->Data[index].buf;}
	public: template<typename TYPE>
	void SetUserData(int index, TYPE* buffer, callback_delete cb = DeleteCB<TYPE>)
	{VarL->Data[index].Set(buffer, cb);}
	public: int GetChildCount() {return VarL->Child.Count();}

	////////////////////////////////////////////////////////////
	// ■ Operators
	////////////////////////////////////////////////////////////
	public: BxPanel& operator[](uint key) {return VarL->Child[key];}
	public: BxPanel& operator()(string key) {return VarL->Child(key);}
	public: BxPanel& operator()(wstring key) {return VarL->Child(key);}

	////////////////////////////////////////////////////////////
	// ■ Inner funtions
	////////////////////////////////////////////////////////////
	private: string OnTouch(Touch type)
	{
		VarL->RefFunc->Service(VarL->VarM);
		return VarL->RefFunc->Touch(type);
	}
    public: void DoTouch(systouchtype type)
	{
		const int X = (VarL->LastRect.l + VarL->LastRect.r) / 2;
		const int Y = (VarL->LastRect.t + VarL->LastRect.b) / 2;
		CalcTouch(type, X, Y);
		BxCore::System::SetSimulatorCursorPos
			(X + BxCore::Main::GetGUIMarginL(), Y + BxCore::Main::GetGUIMarginT());
	}
    public: BxDraw& GetDraw()
	{
		BxASSERT("BxPanel<PaintReady()를 선행호출하지 않았습니다>", Single->RefDraw);
		return *Single->RefDraw;
	}
    public: void UpdateRect(View type = viewNormal)
	{
		VarL->LastType = type;
		VarL->LastRect = BxDrawGlobal::_DrawOption::CurClipRect();
		const int GUIMarginL = BxCore::Main::GetGUIMarginL();
		const int GUIMarginT = BxCore::Main::GetGUIMarginT();
		VarL->LastRect.l -= GUIMarginL;
		VarL->LastRect.t -= GUIMarginT;
		VarL->LastRect.r -= GUIMarginL;
		VarL->LastRect.b -= GUIMarginT;
		static const int XLength = (Single->RefDraw->Width(true) + CellSize - 1) / CellSize;
		static const int YLength = (Single->RefDraw->Height(true) + CellSize - 1) / CellSize;
		const int XBegin = BxUtilGlobal::Max(0, VarL->LastRect.l / CellSize);
		const int YBegin = BxUtilGlobal::Max(0, VarL->LastRect.t / CellSize);
		const int XEnd = BxUtilGlobal::Min(XLength, (VarL->LastRect.r + CellSize - 1) / CellSize);
		const int YEnd = BxUtilGlobal::Min(YLength, (VarL->LastRect.b + CellSize - 1) / CellSize);
		for(int y = YBegin; y < YEnd; ++y)
		for(int x = XBegin; x < XEnd; ++x)
		{
			Singleton::PanelCell& Cell = Single->PanelCells[x + y * XLength];
			if(Cell.WriteID != Single->LastWriteID)
			{
				Cell.WriteID = Single->LastWriteID;
				Cell.Length = 0;
			}
			Cell.Stack[Cell.Length++] = this;
		}
	}
	public: void UpdateScroll(int width, int height, bool force)
	{
		VarL->LastScroll.w = width;
		VarL->LastScroll.h = height;
		if(force)
		{
			if(Single->LastPanel)
				VarL->LastScrollPower = BxUtilGlobal::Max(0, VarL->LastScrollPower * 7 / 8);
			else
			{
				VarL->ForceScroll(this, VarL->LastScrollPhy.x, VarL->LastScrollPhy.y);
				VarL->LastScrollPhy.x = VarL->LastScrollPhy.x * 15 / 16;
				VarL->LastScrollPhy.y = VarL->LastScrollPhy.y * 15 / 16;
			}
		}
	}
	public: point GetScrollPos()
	{
		return XY(VarL->LastScroll.hx, VarL->LastScroll.hy);
	}

	////////////////////////////////////////////////////////////
	// ■ Inner classes
	////////////////////////////////////////////////////////////
	private: class Singleton
	{
		public: class PanelCell
		{
			public: uint WriteID;
			public: int Length;
			public: BxVarVector<BxPanel*, 8> Stack;
			public: PanelCell() : WriteID(0), Length(0) {}
		};
		public: BxDraw* RefDraw;
		public: uint LastWriteID;
		public: BxPanel* LastPanel;
		public: BxPanel* LastPanelScrolled;
		public: point LastTouch[3];
		public: BxVarVector<PanelCell, 4> PanelCells;
		public: Singleton()
		{
			RefDraw = nullptr;
			LastWriteID = 0;
			LastPanel = nullptr;
			LastPanelScrolled = nullptr;
			LastTouch[0] = point::zero();
			LastTouch[1] = point::zero();
			LastTouch[2] = point::zero();
		}
	};
	private: class Function
	{
		public: id_text TypeText;
		public: CreateCB Create;
		public: NotifyCB Notify;
		public: TouchCB Touch;
		public: PaintCB Paint;
		public: AllocCB Alloc;
		public: FreeCB Free;
		public: ServiceCB Service;
		public: global_func Function* Get(string type, bool docreate)
		{
			global_data BxVarMap<Function>* Map = BxNew(BxVarMap<Function>);
			return (docreate)? &(*Map)(type) : Map->Access(type);
		}
	};
	private: class EventData
	{
		public: class Node
		{
			public: EventCB CB;
			public: void* Data;
			public: Node() : CB(nullptr), Data(nullptr) {}
		};
		public: BxVarVector<Node, 1> Callbacks;
		public: void Calling(BxPanel* panel, string message)
		{
			for(int i = 0, iend = Callbacks.Length(); i < iend; ++i)
			{
				Node& CurNode = Callbacks[i];
				if(CurNode.CB)
					CurNode.CB(panel, message, CurNode.Data);
			}
		}
	};
	private: class Event
	{
		private: EventData& Data;
		public: Event(EventData& data) : Data(data) {}
		public: Event& operator+=(EventCB cb)
		{
			Data.Callbacks[LAST].CB = cb;
			return *this;
		}
		public: Event& operator-=(EventCB cb)
		{
			for(int i = Data.Callbacks.Length() - 1; 0 <= i; --i)
				if(Data.Callbacks[i].CB == cb)
				{
					Data.Callbacks.Delete(i);
					break;
				}
			return *this;
		}
		public: Event& operator,(void* data)
		{
			Data.Callbacks[FINAL].Data = data;
			return *this;
		}
	};
	private: class UserData
	{
		public: void* buf;
		public: callback_delete cb;
		public: UserData() : buf(nullptr), cb(nullptr) {}
		public: ~UserData() {if(buf && cb) cb(buf);}
		public: void Set(void* buffer, callback_delete callback)
		{
			if(buf && cb) cb(buf);
			buf = buffer;
			cb = callback;
		}
	};
	private: class VariableL
	{
		public: bool Enable;
		public: bool Visible;
		public: View LastType;
		public: rect LastRect;
		public: area LastScroll;
		public: int LastScrollPower;
		public: point LastScrollPhy;
		public: Function* RefFunc;
		public: BxPanel* VarM;
		public: EventData ClickedData;
		public: EventData BumpedData;
		public: BxVarMap<BxPanel> Child;
		public: BxVarVector<UserData, 4> Data;
		public: int DebugLineHit;
		public: VariableL()
		{
			Enable = true;
			Visible = true;
			LastType = viewUnknown;
			LastRect = rect::zero();
			LastScroll.w = 0;
			LastScroll.h = 0;
			LastScroll.hx = 0;
			LastScroll.hy = 0;
			LastScrollPower = 0;
			LastScrollPhy = point::zero();
			RefFunc = nullptr;
			VarM = nullptr;
			DebugLineHit = 0;
		}
		public: ~VariableL()
		{
			if(RefFunc) RefFunc->Free(&VarM);
		}
		public: void ForceScroll(BxPanel* panel, int fx, int fy)
		{
			const int ScrollW = LastRect.width() - LastScroll.w;
			const int ScrollH = LastRect.height() - LastScroll.h;
			const bool IsVerticalBump = (BxUtilGlobal::Abs(fx) < BxUtilGlobal::Abs(fy));
			if(ScrollW < 0)
			{
				const int NewScrollX = LastScroll.hx + fx / ScrollSensitive;
				if(0 < NewScrollX)
				{
					LastScroll.hx = 0;
					LastScrollPhy.x = fx / 2;
					if(!IsVerticalBump)
						BumpedData.Calling(panel, "Left");
				}
				else if(NewScrollX < ScrollW)
				{
					LastScroll.hx = ScrollW;
					LastScrollPhy.x = fx / 2;
					if(!IsVerticalBump)
						BumpedData.Calling(panel, "Right");
				}
				else
				{
					LastScroll.hx = NewScrollX;
					LastScrollPhy.x = fx;
				}
			}
			else
			{
				LastScroll.hx = 0;
				LastScrollPhy.x = 0;
			}
			if(ScrollH < 0)
			{
				const int NewScrollY = LastScroll.hy + fy / ScrollSensitive;
				if(0 < NewScrollY)
				{
					LastScroll.hy = 0;
					LastScrollPhy.y = fy / 2;
					if(IsVerticalBump)
						BumpedData.Calling(panel, "Top");
				}
				else if(NewScrollY < ScrollH)
				{
					LastScroll.hy = ScrollH;
					LastScrollPhy.y = fy / 2;
					if(IsVerticalBump)
						BumpedData.Calling(panel, "Bottom");
				}
				else
				{
					LastScroll.hy = NewScrollY;
					LastScrollPhy.y = fy;
				}
			}
			else
			{
				LastScroll.hy = 0;
				LastScrollPhy.y = 0;
			}
		}
	};
	public: class VariableS
	{
		public: class Font
		{
			private: id_font id;
			public: Font() : id(nullptr) {}
			public: ~Font() {BxCore::Font::Close(id);}
			public: bool IsLoaded() {return (id != nullptr);}
			public: void Load(string filename, const int pointsize)
			{
				if(!BxCore::Font::IsExistNick(filename))
					BxCore::Font::NickOpen(filename, filename);
				BxCore::Font::Close(id);
				id = BxCore::Font::Open(filename, pointsize);
			}
			public: operator id_font() const
			{
				return id;
			}
		};
		public: BxVarMap<BxImage> img;
		public: BxVarMap<Font> fnt;
		public: BxVarMap<void*> ptr;
		public: BxVarMap<int> flag;
		public: BxVarMap<int> int4;
		public: BxVarMap<huge> int8;
	};
    public: template<typename TYPE>
	class VariableM : public TYPE
	{
		public: BxVarMap<BxImage> img;
		public: BxVarMap<BxString> str;
		public: BxVarMap<void*> ptr;
		public: BxVarMap<int> flag;
		public: BxVarMap<int> int4;
		public: BxVarMap<huge> int8;
		public: VariableM(bool isRef, VariableL* varL) : TYPE(isRef, varL) {}
		public: virtual ~VariableM() {}
	};
	public: class ServiceS
	{
		private: VariableS* RefS;
		public: ServiceS() : RefS(nullptr) {}
		public: ~ServiceS() {}
		public: VariableS& operator--(int) {return *RefS;}
		public: void** Get() {return (void**) &RefS;}
	};
	public: template<typename TYPE>
	class ServiceM
	{
		private: VariableM<TYPE>* RefM;
		public: ServiceM() : RefM(nullptr) {}
		public: ~ServiceM() {}
		public: VariableM<TYPE>& operator--(int) {return *RefM;}
		public: void Set(void* ptr) {RefM = (VariableM<TYPE>*) ptr;}
	};

	////////////////////////////////////////////////////////////
	// ■ Static funtions
	////////////////////////////////////////////////////////////
	public: global_func bool AddPanel(string type, CreateCB c, NotifyCB n, TouchCB t, PaintCB p, AllocCB a, FreeCB f, ServiceCB s)
	{
		BxASSERT("BxPanel<중복된 이름의 패널이 존재합니다>", Function::Get(type, false) == nullptr);
		Function* NewFunction = Function::Get(type, true);
		NewFunction->TypeText = BxCore::Font::TextOpen(type);
		NewFunction->Create = c;
		NewFunction->Notify = n;
		NewFunction->Touch = t;
		NewFunction->Paint = p;
		NewFunction->Alloc = a;
		NewFunction->Free = f;
		NewFunction->Service = s;
		return true;
	}
	private: global_func void PaintReady(BxDraw& draw)
	{
		BxPanel Panel;
		Panel.Single->RefDraw = &draw;
		Panel.Single->LastWriteID++;
	}
	private: global_func bool CalcTouch(systouchtype type, int x, int y)
	{
		BxPanel Panel;
		if(!Panel.Single->RefDraw) return false;
		// Operate last panel
		BxPanel* CurPanel = Panel.Single->LastPanel;
		BxPanel* CurPanelScrolled = Panel.Single->LastPanelScrolled;
		if(CurPanel)
		{
			if(CurPanel == CurPanelScrolled)
			{
				switch(type)
				{
				case systouchtype_move:
					CurPanelScrolled->VarL->ForceScroll(CurPanelScrolled,
						(x - Panel.Single->LastTouch[0].x) * ScrollSensitive,
						(y - Panel.Single->LastTouch[0].y) * ScrollSensitive);
					CurPanelScrolled->VarL->LastScrollPower =
						BxUtilGlobal::Min(CurPanelScrolled->VarL->LastScrollPower + ScrollSensitive / 8, ScrollSensitive);
					Panel.Single->LastTouch[2] = Panel.Single->LastTouch[1];
					Panel.Single->LastTouch[1] = Panel.Single->LastTouch[0];
					Panel.Single->LastTouch[0] = XY(x, y);
					break;
				case systouchtype_up:
				case systouchtype_cancel:
					Panel.Single->LastPanel = nullptr;
					Panel.Single->LastPanelScrolled = nullptr;
					CurPanelScrolled->VarL->LastScrollPhy.x = (x - Panel.Single->LastTouch[2].x) * CurPanelScrolled->VarL->LastScrollPower;
					CurPanelScrolled->VarL->LastScrollPhy.y = (y - Panel.Single->LastTouch[2].y) * CurPanelScrolled->VarL->LastScrollPower;
					break;
				}
				return true;
			}
			else
			{
				const rect& Rect = CurPanel->VarL->LastRect;
				bool IsOutside = (x < Rect.l || Rect.r <= x || y < Rect.t || Rect.b <= y);
				switch(type)
				{
				case systouchtype_move:
					if(CurPanelScrolled)
					{
						Panel.Single->LastPanel = CurPanelScrolled;
						Panel.Single->LastTouch[0] = Panel.Single->LastTouch[1] = Panel.Single->LastTouch[2] = XY(x, y);
						CurPanel->OnTouch(tchUpAuto);
						return true;
					}
					else CurPanel->OnTouch((IsOutside)? tchMoveOut : tchMoveIn);
					break;
				case systouchtype_down: IsOutside = false; // For key pressed
				case systouchtype_up:
				case systouchtype_cancel:
					{
						Panel.Single->LastPanel = nullptr;
						Panel.Single->LastPanelScrolled = nullptr;
						string Message = CurPanel->OnTouch((IsOutside)? tchUpOut : tchUpIn);
						if(!IsOutside) CurPanel->VarL->ClickedData.Calling(CurPanel, Message);
					}
					break;
				}
			}
		}
		// Look into new panel
		static const int XLength = (Panel.Single->RefDraw->Width(true) + CellSize - 1) / CellSize;
		static const int YLength = (Panel.Single->RefDraw->Height(true) + CellSize - 1) / CellSize;
		const int X = x / CellSize, Y = y / CellSize;
		if(X < 0 || XLength <= X || Y < 0 || YLength <= Y) return false;
		Singleton::PanelCell& Cell = Panel.Single->PanelCells[X + Y * XLength];
		if(Cell.WriteID != Panel.Single->LastWriteID) return false;
		for(int i = Cell.Length - 1; 0 <= i; --i)
		{
			const rect& Rect = Cell.Stack[i]->VarL->LastRect;
			if(x < Rect.l || Rect.r <= x || y < Rect.t || Rect.b <= y)
				continue;
			if(CurPanel != Cell.Stack[i])
			{
				CurPanel = Cell.Stack[i];
				CurPanelScrolled = nullptr;
				switch(type)
				{
				case systouchtype_down:
					// Search scrolled panel at under new panel
					while(0 <= --i)
					{
						VariableL* CurL = (VariableL*) Cell.Stack[i]->VarL;
						const rect& Rect = CurL->LastRect;
						if(x < Rect.l || Rect.r <= x || y < Rect.t || Rect.b <= y)
							continue;
						if(CurL->LastType == viewScroll)
						{
							CurPanelScrolled = Cell.Stack[i];
							break;
						}
					}
					Panel.Single->LastPanel = CurPanel;
					Panel.Single->LastPanelScrolled = CurPanelScrolled;
					CurPanel->OnTouch(tchDown);
					CurPanel->VarL->ForceScroll(CurPanel, 0, 0);
					break;
				case systouchtype_move:
					CurPanel->OnTouch(tchDrag);
					break;
				case systouchtype_up:
				case systouchtype_cancel:
					CurPanel->OnTouch(tchDrop);
					break;
				}
			}
			return true;
		}
		return (CurPanel != nullptr);
	}
	public: global_func void HitupDebugLine(int x, int y)
	{
		BxPanel Panel;
		if(!Panel.Single->RefDraw) return;
		static const int XLength = (Panel.Single->RefDraw->Width(true) + CellSize - 1) / CellSize;
		static const int YLength = (Panel.Single->RefDraw->Height(true) + CellSize - 1) / CellSize;
		const int X = x / CellSize, Y = y / CellSize;
		if(X < 0 || XLength <= X || Y < 0 || YLength <= Y) return;
		Singleton::PanelCell& Cell = Panel.Single->PanelCells[X + Y * XLength];
		if(Cell.WriteID != Panel.Single->LastWriteID) return;
		for(int i = Cell.Length - 1; 0 <= i; --i)
		{
			BxPanel* Panel = Cell.Stack[i];
			const rect& Rect = Panel->VarL->LastRect;
			if(x < Rect.l || Rect.r <= x || y < Rect.t || Rect.b <= y)
				continue;
			Cell.Stack[i]->VarL->DebugLineHit = 8;
			return;
		}
	}
	private: template<typename TYPE>
	global_func void DeleteCB(void* data) {delete (TYPE*) data;}

	////////////////////////////////////////////////////////////
	// ■ Constructors
	////////////////////////////////////////////////////////////
	public: BxPanel(bool isRef = false, void* varL = new VariableL())
		: IsRef(isRef), VarL((VariableL*) varL), Clicked(VarL->ClickedData), Bumped(VarL->BumpedData)
	{
		global_data Singleton* S = BxNew(Singleton);
		Single = S;
	}
	public: virtual ~BxPanel()
	{
		if(!IsRef) delete VarL;
	}

	////////////////////////////////////////////////////////////
	// ■ Variables
	////////////////////////////////////////////////////////////
	private: Singleton* Single;
	private: const bool IsRef;
    public: VariableL* const VarL;
	public: Event Clicked;
	public: Event Bumped;
};

#define FRAMEWORK_PANEL(NAME, PARAM) FRAMEWORK_PANEL_CLASS(NAME, BxPanel, PARAM)
#define FRAMEWORK_PANEL_CLASS(NAME, CLASS, PARAM) \
	local_func void OnCreate(string, unknown, const bool); \
	local_func unknown OnNotify(string, unknown); \
	local_func string OnTouch(BxPanel::Touch); \
	local_func void OnPaint(rect&, int, int, int, int); \
	local_func void _Create(string option, unknown param) \
	{ \
		global_data bool IsFirstCreate = true; \
		OnCreate(option, param, IsFirstCreate); \
		IsFirstCreate = false; \
	} \
	local_func unknown _Notify(string message, unknown param) \
	{ \
		return OnNotify(message, param); \
	} \
	local_func string _Touch(BxPanel::Touch type) \
	{ \
		return OnTouch(type); \
	} \
	local_func void _Paint(BxPanel::Param type, const BxArgument& args, rect& result) \
	{ \
		BxASSERT("BxPanel<해당 패널에서 지원하지 않는 type입니다>", type == BxPanel::prm##PARAM); \
		const int* prm1 = args.Access<int>(0); \
		const int* prm2 = args.Access<int>(1); \
		const int* prm3 = args.Access<int>(2); \
		const int* prm4 = args.Access<int>(3); \
		switch(type) \
		{ \
		case BxPanel::prmXY: \
			BxASSERT("BxPanel<type에 맞는 인수형태가 아닙니다>", prm1 && prm2 && !prm3 && !prm4); \
			OnPaint(result, *prm1, *prm2, 0, 0); \
			break; \
		case BxPanel::prmXYW: \
			BxASSERT("BxPanel<type에 맞는 인수형태가 아닙니다>", prm1 && prm2 && prm3 && !prm4); \
			OnPaint(result, *prm1, *prm2, *prm3, 0); \
			break; \
		case BxPanel::prmXYXY: case BxPanel::prmXYWH: \
			BxASSERT("BxPanel<type에 맞는 인수형태가 아닙니다>", prm1 && prm2 && prm3 && prm4); \
			OnPaint(result, *prm1, *prm2, *prm3, *prm4); \
			break; \
		} \
	} \
	local_func BxPanel* _Alloc(BxPanel* original) \
	{ \
		return new BxPanel::VariableM<CLASS>(true, original->VarL); \
	} \
	local_func void _Free(BxPanel** ptr) \
	{ \
		delete *ptr; \
		*ptr = nullptr; \
	} \
	local_data BxPanel::ServiceS s; \
	local_data BxPanel::ServiceM<CLASS> m; \
	local_func void** _Service(void* ptr) \
	{ \
		m.Set(ptr); \
		return s.Get(); \
	} \
	local_data bool unknown_panel = BxPanel::AddPanel("" NAME, _Create, _Notify, _Touch, _Paint, _Alloc, _Free, _Service);
