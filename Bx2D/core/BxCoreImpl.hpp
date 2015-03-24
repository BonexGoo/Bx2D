#pragma once
#include <BxKeyword.hpp>

namespace BxCore
{
	namespace Main
	{
		/// @cond SECTION_NAME
		int& ForMain_LetUpdateCount() {global_data int _ = 0; return _;}
		int& ForMain_LetRenderCount() {global_data int _ = 0; return _;}
		int& ForMain_LetFrameTime() {global_data int _ = 20; return _;}
		int& ForMain_RealFrameTime() {global_data int _ = 20; return _;}
		int& ForMain_LetFrameDelay() {global_data int _ = 0; return _;}
		rect& ForMain_LetGUIMargin() {global_data rect _ = {0, 0, 0, 0}; return _;}
		void*& ForMain_LetDrawPtr() {global_data void* _ = nullptr; return _;}
		bool& ForMain_LetIsChildProcessRun() {global_data bool _ = false; return _;}
		/// @endcond

		/// @cond SECTION_NAME
		bool _CertifyGUIMarginLoad()
		{
			global_data bool IsLoaded = false;
			if(!IsLoaded)
			{
				IsLoaded = true;
				if(IsEnableGUI())
				{
					ForMain_LetGUIMargin().l = BxCore::System::GetConfigNumber("Bx.Framework.GUIMargin.Left", 0);
					ForMain_LetGUIMargin().t = BxCore::System::GetConfigNumber("Bx.Framework.GUIMargin.Top", 0);
					ForMain_LetGUIMargin().r = BxCore::System::GetConfigNumber("Bx.Framework.GUIMargin.Right", 0);
					ForMain_LetGUIMargin().b = BxCore::System::GetConfigNumber("Bx.Framework.GUIMargin.Bottom", 0);
				}
			}
			return true;
		}
		/// @endcond
	}

	namespace System
	{
		/// @cond SECTION_NAME
		bool& ForSystem_LetEnablePanelDebug() {global_data bool _ = false; return _;}
		bool& ForSystem_LetEnableFontDebug() {global_data bool _ = false; return _;}
		/// @endcond

		/// @cond SECTION_NAME
		bool _CertifyConfigLoad()
		{
			global_data bool IsLoaded = false;
			if(!IsLoaded)
			{
				IsLoaded = true;
				const bool LoadResult = BxKeyword::Load("config/default.key");
				BxASSERT("BxCore::System<config/default.key파일을 로드할 수 없습니다>", LoadResult);
			}
			return true;
		}
		/// @endcond

		/// @cond SECTION_NAME
		class CallbackList
		{
		public:
			callback_frame Method;
			bool IsDataRef;
			int Value;
			void* Data;
			CallbackList* Next;
		public:
			CallbackList(callback_frame method, int value, void* data, int datasize)
			{
				Method = method;
				Value = value;
				IsDataRef = (datasize <= -1);
				if(!IsDataRef)
				{
					if(0 < datasize && data)
					{
						Data = BxAlloc(datasize);
						BxCore::Util::MemCpy(Data, data, datasize);
					}
					else Data = nullptr;
				}
				else Data = data;
				Next = nullptr;
			}
			virtual ~CallbackList()
			{
				if(!IsDataRef && Data) BxFree(Data);
				if(Next) BxDelete(Next);
			}
			global_func CallbackList* GetFirst()
			{
				global_data CallbackList First(nullptr, 0, nullptr, -1);
				return &First;
			}
		};
		/// @endcond

		/// @cond SECTION_NAME
		void ForSystem_CallbackRun()
		{
			CallbackList* List = CallbackList::GetFirst();
			while(List->Next)
			{
				if(!List->Next->Method(List->Next->Value, List->Next->Data))
				{
					CallbackList* DeleteList = List->Next;
					List->Next = DeleteList->Next;
					DeleteList->Next = nullptr;
					BxDelete(DeleteList);
				}
				else List = List->Next;
			}
		}
		/// @endcond

		/// @cond SECTION_NAME
		local_func inline callback_process& _ChildProcess() {global_data callback_process ChildProcess = nullptr; return ChildProcess;}
		void ForSystem_ClearChildProcess()
		{
			_ChildProcess() = nullptr;
		}
		bool ForSystem_HasChildProcess()
		{
			return (_ChildProcess() != nullptr);
		}
		void ForSystem_RunChildProcess(callback_procedure yield)
		{
			BxASSERT("BxCore::System::_RunChildProcess<등록된 ChildProcess가 없습니다>", _ChildProcess());
			_ChildProcess()(yield);
		}
		/// @endcond

		/// @cond SECTION_NAME
		local_func inline callback_edk& _EDKClient() {global_data callback_edk EDKClient = nullptr; return EDKClient;}
		bool _EDKCallback(int value, void* data)
		{
			if(_EDKClient())
				_EDKClient()((string) data, value);
			return false;
		}
		void _EDKReceiver(string param1_str, int param2)
		{
			AddCallback(_EDKCallback, param2, (void*) param1_str, BxUtilGlobal::StrLen(param1_str) + 1);
		}
		/// @endcond
	}

	namespace AddOn
	{
		/// @cond SECTION_NAME
		typedef const byte* (*CoreJPGToBMP)(const byte*);
		const byte* DefaultJPGToBMP(const byte*)
		{BxASSERT("BxCore::AddOn<해당 JPGToBMP확장함수가 없습니다>", false); return nullptr;}
		void* _inout_ ForExtend_JPGToBMP() {static void* Function = (void*) DefaultJPGToBMP; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef const byte* (*CoreGIFToBMP)(const byte*, const int, int _out_);
		const byte* DefaultGIFToBMP(const byte*, const int, int _out_)
		{BxASSERT("BxCore::AddOn<해당 GIFToBMP확장함수가 없습니다>", false); return nullptr;}
		void* _inout_ ForExtend_GIFToBMP() {static void* Function = (void*) DefaultGIFToBMP; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef const byte* (*CoreHQXToBMP)(const byte*, int);
		const byte* DefaultHQXToBMP(const byte*, int)
		{BxASSERT("BxCore::AddOn<해당 HQXToBMP확장함수가 없습니다>", false); return nullptr;}
		void* _inout_ ForExtend_HQXToBMP() {static void* Function = (void*) DefaultHQXToBMP; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef string _tmp_ (*CoreBUFToMD5)(const byte*, const int);
		string _tmp_ DefaultBUFToMD5(const byte*, const int)
		{BxASSERT("BxCore::AddOn<해당 BUFToMD5확장함수가 없습니다>", false); return nullptr;}
		void* _inout_ ForExtend_BUFToMD5() {static void* Function = (void*) DefaultBUFToMD5; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef id_zip (*CoreCreateZIP)(const byte*, const int, int _out_, string);
		id_zip DefaultCreateZIP(const byte*, const int, int _out_, string)
		{BxASSERT("BxCore::AddOn<해당 CreateZIP확장함수가 없습니다>", false); return nullptr;}
		void* _inout_ ForExtend_CreateZIP() {static void* Function = (void*) DefaultCreateZIP; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef void (*CoreReleaseZIP)(id_zip);
		void DefaultReleaseZIP(id_zip)
		{BxASSERT("BxCore::AddOn<해당 ReleaseZIP확장함수가 없습니다>", false);}
		void* _inout_ ForExtend_ReleaseZIP() {static void* Function = (void*) DefaultReleaseZIP; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef const byte* (*CoreZIPToFILE)(id_zip, const int, int _out_);
		const byte* DefaultZIPToFILE(id_zip, const int, int _out_)
		{BxASSERT("BxCore::AddOn<해당 ZIPToFILE확장함수가 없습니다>", false); return nullptr;}
		void* _inout_ ForExtend_ZIPToFILE() {static void* Function = (void*) DefaultZIPToFILE; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef string _tmp_ (*CoreZIPToINFO)(id_zip, const int, bool*, uhuge*, uhuge*, uhuge*, bool*, bool*, bool*, bool*);
		string _tmp_ DefaultZIPToINFO(id_zip, const int, bool*, uhuge*, uhuge*, uhuge*, bool*, bool*, bool*, bool*)
		{BxASSERT("BxCore::AddOn<해당 ZIPToINFO확장함수가 없습니다>", false); return nullptr;}
		void* _inout_ ForExtend_ZIPToINFO() {static void* Function = (void*) DefaultZIPToINFO; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef id_ttf (*CoreCreateTTF)(const byte*, const int);
		id_ttf DefaultCreateTTF(const byte*, const int)
		{BxASSERT("BxCore::AddOn<해당 CreateTTF확장함수가 없습니다>", false); return nullptr;}
		void* _inout_ ForExtend_CreateTTF() {static void* Function = (void*) DefaultCreateTTF; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef void (*CoreReleaseTTF)(id_ttf);
		void DefaultReleaseTTF(id_ttf)
		{BxASSERT("BxCore::AddOn<해당 ReleaseTTF확장함수가 없습니다>", false);}
		void* _inout_ ForExtend_ReleaseTTF() {static void* Function = (void*) DefaultReleaseTTF; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef const byte* (*CoreTTFToBMP)(id_ttf, int, uint);
		const byte* DefaultTTFToBMP(id_ttf, int, uint)
		{BxASSERT("BxCore::AddOn<해당 TTFToBMP확장함수가 없습니다>", false); return nullptr;}
		void* _inout_ ForExtend_TTFToBMP() {static void* Function = (void*) DefaultTTFToBMP; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef bool (*CoreTTFToINFO)(id_ttf, int, uint, int*, int*);
		bool DefaultTTFToINFO(id_ttf, int, uint, int*, int*)
		{BxASSERT("BxCore::AddOn<해당 TTFToINFO확장함수가 없습니다>", false); return nullptr;}
		void* _inout_ ForExtend_TTFToINFO() {static void* Function = (void*) DefaultTTFToINFO; return Function;}
		/// @endcond
	}

	namespace Wacom
	{
		/// @cond SECTION_NAME
		#define WT_DEFBASE			0x7FF0
		#define WT_MAXOFFSET		0xF
		#define _WT_PACKET(b)		((b)+0)
		#define _WT_CTXOPEN(b)		((b)+1)
		#define _WT_CTXCLOSE(b)		((b)+2)
		#define _WT_CTXUPDATE(b)	((b)+3)
		#define _WT_CTXOVERLAP(b)	((b)+4)
		#define _WT_PROXIMITY(b)	((b)+5)
		#define _WT_INFOCHANGE(b)	((b)+6)
		#define _WT_CSRCHANGE(b)	((b)+7) // 1.1
		#define _WT_PACKETEXT(b)	((b)+8) // 1.4
		#define _WT_MAX(b)			((b)+WT_MAXOFFSET)
		#define WT_PACKET			_WT_PACKET(WT_DEFBASE)
		#define WT_CTXOPEN			_WT_CTXOPEN(WT_DEFBASE)
		#define WT_CTXCLOSE			_WT_CTXCLOSE(WT_DEFBASE)
		#define WT_CTXUPDATE		_WT_CTXUPDATE(WT_DEFBASE)
		#define WT_CTXOVERLAP		_WT_CTXOVERLAP(WT_DEFBASE)
		#define WT_PROXIMITY		_WT_PROXIMITY(WT_DEFBASE)
		#define WT_INFOCHANGE		_WT_INFOCHANGE(WT_DEFBASE)
		#define WT_CSRCHANGE		_WT_CSRCHANGE(WT_DEFBASE) // 1.1
		#define WT_PACKETEXT		_WT_PACKETEXT(WT_DEFBASE) // 1.4
		#define WT_MAX				_WT_MAX(WT_DEFBASE)
		#define WTI_DEFCONTEXT		3
		#define WTI_DEFSYSCTX		4
		#define WTI_DDCTXS			400 // 1.1
		#define WTI_DSCTXS			500 // 1.1
		#define CXO_SYSTEM			0x0001
		#define CXO_PEN				0x0002
		#define CXO_MESSAGES		0x0004
		#define CXO_MARGIN			0x8000
		#define CXO_MGNINSIDE		0x4000
		#define CXO_CSRMESSAGES		0x0008 // 1.1
		#define PK_BUTTONS			0x0040
		#define PK_X				0x0080
		#define PK_Y				0x0100
		#define PK_Z				0x0200
		#define PK_NORMAL_PRESSURE	0x0400
		#define PACKETDATA			PK_BUTTONS | PK_X | PK_Y | PK_Z | PK_NORMAL_PRESSURE
		#define PACKETMODE			0

		DECLARE_HND(HWND);
		DECLARE_HND(HCTX);
		typedef int BOOL;
		typedef uint FIX32;
		typedef uint WTPKT;

		typedef struct tag_PACKET {
			uint    pkButtons;
			long    pkX;
			long    pkY;
			long    pkZ;
			uint    pkNormalPressure;
		} PACKET;

		typedef struct tag_LOGCONTEXTA {
			char    lcName[40];
			uint    lcOptions;
			uint    lcStatus;
			uint    lcLocks;
			uint    lcMsgBase;
			uint    lcDevice;
			uint    lcPktRate;
			WTPKT   lcPktData;
			WTPKT   lcPktMode;
			WTPKT   lcMoveMask;
			uint    lcBtnDnMask;
			uint    lcBtnUpMask;
			long    lcInOrgX, lcInOrgY, lcInOrgZ;
			long    lcInExtX, lcInExtY, lcInExtZ;
			long    lcOutOrgX, lcOutOrgY, lcOutOrgZ;
			long    lcOutExtX, lcOutExtY, lcOutExtZ;
			FIX32   lcSensX, lcSensY, lcSensZ;
			BOOL    lcSysMode;
			int     lcSysOrgX, lcSysOrgY;
			int     lcSysExtX, lcSysExtY;
			FIX32   lcSysSensX, lcSysSensY;
		} LOGCONTEXTA, *PLOGCONTEXTA, *NPLOGCONTEXTA, *LPLOGCONTEXTA;
        typedef LOGCONTEXTA LOGCONTEXT;
        typedef PLOGCONTEXTA PLOGCONTEXT;
        typedef NPLOGCONTEXTA NPLOGCONTEXT;
        typedef LPLOGCONTEXTA LPLOGCONTEXT;

        #ifdef ANDROID
            #define STDCALL
        #else
            #define STDCALL __stdcall
        #endif
        typedef uint (STDCALL *WTINFOA)(uint, uint, void*);
        typedef HCTX (STDCALL *WTOPENA)(HWND, LPLOGCONTEXTA, BOOL);
        typedef BOOL (STDCALL *WTGETA)(HCTX, LPLOGCONTEXT);
        typedef BOOL (STDCALL *WTSETA)(HCTX, LPLOGCONTEXT);
        typedef BOOL (STDCALL *WTCLOSE)(HCTX);
        typedef BOOL (STDCALL *WTENABLE)(HCTX, BOOL);
        typedef BOOL (STDCALL *WTPACKET)(HCTX, uint, void*);
        typedef BOOL (STDCALL *WTOVERLAP)(HCTX, BOOL);
        typedef BOOL (STDCALL *WTSAVE)(HCTX, void*);
        typedef BOOL (STDCALL *WTCONFIG)(HCTX, HWND);
        typedef HCTX (STDCALL *WTRESTORE)(HWND, void*, BOOL);
        typedef BOOL (STDCALL *WTEXTSET)(HCTX, uint, void*);
        typedef BOOL (STDCALL *WTEXTGET)(HCTX, uint, void*);
        typedef BOOL (STDCALL *WTQUEUESIZESET)(HCTX, int);
        typedef int (STDCALL *WTDATAPEEK)(HCTX, uint, uint, int, void*, int*);
        typedef int (STDCALL *WTPACKETSGET)(HCTX, int, void*);

		template<typename TYPE, bool FORTHREAD = true, int MAXCOUNT = 256>
		class BxVarQueue
		{
		private:
			class QueueElement
			{
			public:
				TYPE* Data;
				QueueElement* Prev;
				QueueElement* Next;
				QueueElement(TYPE* data) : Data(data), Prev(this), Next(this) {}
				~QueueElement()
				{
					if(Prev) Prev->Next = nullptr;
					QueueElement* CurrNode = nullptr;
					QueueElement* NextNode = Next;
					while(CurrNode = NextNode)
					{
						NextNode = CurrNode->Next;
						CurrNode->Prev = nullptr;
						CurrNode->Next = nullptr;
						delete CurrNode;
					}
				}
				void Enqueue(TYPE* data)
				{
					QueueElement* NewNode = new QueueElement(data);
					(NewNode->Prev = Prev)->Next = NewNode;
					(NewNode->Next = this)->Prev = NewNode;
				}
				TYPE* Dequeue()
				{
					QueueElement* DeleteNode = Next;
					DeleteNode->Next->Prev = DeleteNode->Prev;
					DeleteNode->Prev->Next = DeleteNode->Next;
					DeleteNode->Prev = nullptr;
					DeleteNode->Next = nullptr;
					TYPE* Result = DeleteNode->Data;
					delete DeleteNode;
					return Result;
				}
			};

		private:
			QueueElement Head;
			int DataCount;
			id_mutex Mutex;

		public:
			BxVarQueue() : Head(nullptr)
			{
				DataCount = 0;
				if(FORTHREAD) Mutex = BxCore::Thread::OpenMutex();
			}

			~BxVarQueue()
			{
				BxASSERT("BxVarQueue<큐에 해제되지 않은 데이터가 존재합니다>", DataCount == 0);
				if(FORTHREAD) BxCore::Thread::CloseMutex(Mutex);
			}

		public:
			void Enqueue(TYPE* data)
			{
				if(FORTHREAD) BxCore::Thread::Lock(Mutex);
				{
					Head.Enqueue(data);
					DataCount++;
					BxASSERT("BxVarQueue<큐의 데이터수량이 경고수준입니다>", DataCount != MAXCOUNT);
				}
				if(FORTHREAD) BxCore::Thread::Unlock(Mutex);
			}

			TYPE* Dequeue()
			{
				TYPE* Data = nullptr;
				if(FORTHREAD) BxCore::Thread::Lock(Mutex);
				if(0 < DataCount)
				{
					Data = Head.Dequeue();
					DataCount--;
				}
				if(FORTHREAD) BxCore::Thread::Unlock(Mutex);
				return Data;
			}
		};

		class WacomSingletonClass
		{
		public:
			id_library Library;
			HCTX Handle;
			LOGCONTEXT Context;
			PACKET* LastPacket;
			BxVarQueue<PACKET, false> PacketQueue;
			WTINFOA WTInfoA;
			WTOPENA WTOpenA;
			WTGETA WTGetA;
			WTSETA WTSetA;
			WTCLOSE WTClose;
			WTPACKET WTPacket;
			WTENABLE WTEnable;
			WTOVERLAP WTOverlap;
			WTSAVE WTSave;
			WTCONFIG WTConfig;
			WTRESTORE WTRestore;
			WTEXTSET WTExtSet;
			WTEXTGET WTExtGet;
			WTQUEUESIZESET WTQueueSizeSet;
			WTDATAPEEK WTDataPeek;
			WTPACKETSGET WTPacketsGet;

		public:
			WacomSingletonClass()
			{
				Library = BxCore::Library::Open("Wintab32.dll");
				Handle = nullptr;
				LastPacket = new PACKET();
                WTInfoA = (WTINFOA) BxCore::Library::Link(Library, "WTInfoA", (void*) NULL_WTINFOA);
                WTOpenA = (WTOPENA) BxCore::Library::Link(Library, "WTOpenA", (void*) NULL_WTOPENA);
                WTGetA = (WTGETA) BxCore::Library::Link(Library, "WTGetA", (void*) NULL_WTGETA);
                WTSetA = (WTSETA) BxCore::Library::Link(Library, "WTSetA", (void*) NULL_WTSETA);
                WTClose = (WTCLOSE) BxCore::Library::Link(Library, "WTClose", (void*) NULL_WTCLOSE);
                WTPacket = (WTPACKET) BxCore::Library::Link(Library, "WTPacket", (void*) NULL_WTPACKET);
                WTEnable = (WTENABLE) BxCore::Library::Link(Library, "WTEnable", (void*) NULL_WTENABLE);
                WTOverlap = (WTOVERLAP) BxCore::Library::Link(Library, "WTOverlap", (void*) NULL_WTOVERLAP);
                WTSave = (WTSAVE) BxCore::Library::Link(Library, "WTSave", (void*) NULL_WTSAVE);
                WTConfig = (WTCONFIG) BxCore::Library::Link(Library, "WTConfig", (void*) NULL_WTCONFIG);
                WTRestore = (WTRESTORE) BxCore::Library::Link(Library, "WTRestore", (void*) NULL_WTRESTORE);
                WTExtSet = (WTEXTSET) BxCore::Library::Link(Library, "WTExtSet", (void*) NULL_WTEXTSET);
                WTExtGet = (WTEXTGET) BxCore::Library::Link(Library, "WTExtGet", (void*) NULL_WTEXTGET);
                WTQueueSizeSet = (WTQUEUESIZESET) BxCore::Library::Link(Library, "WTQueueSizeSet", (void*) NULL_WTQUEUESIZESET);
                WTDataPeek = (WTDATAPEEK) BxCore::Library::Link(Library, "WTDataPeek", (void*) NULL_WTDATAPEEK);
                WTPacketsGet = (WTPACKETSGET) BxCore::Library::Link(Library, "WTPacketsGet", (void*) NULL_WTPACKETSGET);
				if(WTInfoA(0, 0, nullptr))
				{
					WTInfoA(WTI_DEFCONTEXT, 0, &Context);
					Context.lcPktData = PACKETDATA;
					Context.lcPktMode = PACKETMODE;
					Context.lcOptions = CXO_MESSAGES;
					void* WindowHandle = BxCore::Simulator::GetWindowHandle();
					Handle = WTOpenA((HWND) WindowHandle, &Context, true);
					BxCore::Simulator::HookEvent(WT_PACKET, OnPacket, this);
				}
			}
			~WacomSingletonClass()
			{
				if(Handle) WTClose(Handle);
				BxCore::Library::Close(Library);
				delete LastPacket;
				for(PACKET* UnusedPacket = nullptr; UnusedPacket = PacketQueue.Dequeue();)
					delete UnusedPacket;
			}

		private:
			global_func void OnPacket(uint message, uint wparam, mint lparam, void* data)
			{
				WacomSingletonClass* This = (WacomSingletonClass*) data;
				PACKET* NewPacket = new PACKET();
				This->WTPacket((HCTX) lparam, wparam, NewPacket);
				This->PacketQueue.Enqueue(NewPacket);
			}

		private:
			global_func uint NULL_WTINFOA(uint, uint, void*) {return 0;}
			global_func HCTX NULL_WTOPENA(HWND, LPLOGCONTEXTA, BOOL) {return nullptr;}
			global_func BOOL NULL_WTGETA(HCTX, LPLOGCONTEXT) {return false;}
			global_func BOOL NULL_WTSETA(HCTX, LPLOGCONTEXT) {return false;}
			global_func BOOL NULL_WTCLOSE(HCTX) {return false;}
			global_func BOOL NULL_WTENABLE(HCTX, BOOL) {return false;}
			global_func BOOL NULL_WTPACKET(HCTX, uint, void*) {return false;}
			global_func BOOL NULL_WTOVERLAP(HCTX, BOOL) {return false;}
			global_func BOOL NULL_WTSAVE(HCTX, void*) {return false;}
			global_func BOOL NULL_WTCONFIG(HCTX, HWND) {return false;}
			global_func HCTX NULL_WTRESTORE(HWND, void*, BOOL) {return nullptr;}
			global_func BOOL NULL_WTEXTSET(HCTX, uint, void*) {return false;}
			global_func BOOL NULL_WTEXTGET(HCTX, uint, void*) {return false;}
			global_func BOOL NULL_WTQUEUESIZESET(HCTX, int) {return false;}
			global_func int NULL_WTDATAPEEK(HCTX, uint, uint, int, void*, int*) {return 0;}
			global_func int NULL_WTPACKETSGET(HCTX, int, void*) {return 0;}
		};
		SINGLETON_CLASS(WacomSingletonClass, WacomSingleton);
		/// @endcond
	}
}
