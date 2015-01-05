// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
#include <BxCoreForQT5.hpp>
#include <BxCore.hpp>

// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
#include <BxAutoBuffer.hpp>
#include <BxExpress.hpp>
#include <BxKeyword.hpp>
#include <BxMemory.hpp>
#include <BxScene.hpp>
#include <BxUtil.hpp>
#include <BxString.hpp>

// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
#ifdef __BX_APPLICATION
	int main(int argc, char** argv)
	{
        QApplication App(argc, argv);
		BxCore::Main::GLWidget& Widget = BxCore::Main::GLWidget::Me();
        #if defined(WIN32) || defined(Q_OS_MACX)
			const int GUIMarginL = BxCore::Main::GetGUIMarginL();
			const int GUIMarginT = BxCore::Main::GetGUIMarginT();
			const int GUIMarginR = BxCore::Main::GetGUIMarginR();
			const int GUIMarginB = BxCore::Main::GetGUIMarginB();
			const int WindowsWidth = BxCore::System::GetConfigNumber("Bx.Application.Windows.Width", 480);
			const int WindowsHeight = BxCore::System::GetConfigNumber("Bx.Application.Windows.Height", 320);
			Widget.SetWindowRect(0, 0,
                GUIMarginL + WindowsWidth + GUIMarginR,
				GUIMarginT + WindowsHeight + GUIMarginB);
            Widget.showNormal();
		#else
			Widget.showFullScreen();
		#endif
        return App.exec();
	}
#endif

#ifdef ANDROID
	extern "C" int __android_log_print(int prio, const char *tag,  const char *fmt, ...);
#endif

// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
namespace BxCore
{
	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Main
	{
        void EventAttach()
		{
		}

		void EventDetach()
		{
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace System
	{
        local_data QSemaphore QuitFlag(1);

		bool IsQuit()
		{
            return (QuitFlag.available() == 0);
		}

		void DoQuit()
		{
            QuitFlag.tryAcquire();
		}

		void Sleep(int ms)
		{
			uhuge StartTime = GetTimeMilliSecond();
			QEventLoop EventLoop(QThread::currentThread());
			EventLoop.processEvents();
			int SleepTime = ms - ((int) (GetTimeMilliSecond() - StartTime));
			if(0 < SleepTime) QThread::msleep(SleepTime);
		}

		uhuge GetTimeMilliSecond()
		{
			return (uhuge) QTime::currentTime().msecsSinceStartOfDay();
		}

		uhuge GetTimeNanoSecond()
		{
			return GetTimeMilliSecond() * 1000;
		}

		uhuge GetTimeSimilarMicroSecond()
		{
			return GetTimeNanoSecond() >> 10;
		}

		bool IsMultiTouchAvailable()
		{
			////////////////////////////////////////
			return false;
		}

		bool IsTouchDown()
		{
            return BxCore::Main::GLWidget::Me().IsPressed();
		}

		bool IsKeyDown(keykind key)
		{
			////////////////////////////////////////
			return false;
		}

		int GetPlatformConfigNumber(string name)
		{
			int Result = -1;
			////////////////////////////////////////
			if(BxUtilGlobal::StrICmp(name, "MemSizeBx") == same)
				Result = 50000000;
			////////////////////////////////////////
			return Result;
		}

		string _tmp_ GetPlatformConfigString(string name)
		{
			global_data char Result[256] = {'\0',};
			////////////////////////////////////////
			return Result;
		}

		void SetSimulatorWindowPos(int x, int y)
		{
            BxCore::Main::GLWidget::Me().move(x, y);
		}

		point GetSimulatorWindowPos()
		{
            QPoint Point = BxCore::Main::GLWidget::Me().pos();
            point Result;
            Result.x = Point.x();
            Result.y = Point.y();
            return Result;
		}

		void SetSimulatorCursorPos(int x, int y)
		{
			QCursor Cursor;
			point WindowPos = GetSimulatorWindowPos();
			Cursor.setPos(QPoint(WindowPos.x + x, WindowPos.y + y));
            BxCore::Main::GLWidget::Me().setCursor(Cursor);
		}

		point GetSimulatorCursorPos()
		{
            QPoint Point = BxCore::Main::GLWidget::Me().cursor().pos();
            point Result;
            Result.x = Point.x();
            Result.y = Point.y();
            return Result;
		}

		void DoSimulatorMinimize()
		{
            BxCore::Main::GLWidget::Me().showMinimized();
		}

		assertresult Break(string name, string query __DEBUG_PRM__)
		{
            #ifdef WIN32
                #define BREAK_TEXT(STRING) BxCore::System::GetUTF8ByCP949(STRING)
            #else
				#define BREAK_TEXT(STRING) (STRING)
            #endif
			#ifdef __BX_DEBUG
				QMessageBox AssertBox(QMessageBox::Warning, "ASSERT BREAK", BREAK_TEXT(name),
					QMessageBox::Yes | QMessageBox::No | QMessageBox::NoToAll);
				QString AssertMessage;
				AssertMessage.sprintf(
					"[QUERY] %s\t\t\n"
					"[METHOD] %s()\t\t\n"
					"[FILE/LINE] %s(%dLn)\t\t\n"
					"[THREAD_ID] %d\t\t\n"
					"\n"
					"(YES is Break, NO is Ignore)\t\t",
					BREAK_TEXT(query),
					__DEBUG_FUNC__, __DEBUG_FILE__, __DEBUG_LINE__, QThread::currentThreadId());
				AssertBox.setInformativeText(AssertMessage);
				AssertBox.setDefaultButton(QMessageBox::Yes);
				switch(AssertBox.exec())
				{
				case QMessageBox::Yes: return assertresult_yes;
				case QMessageBox::NoToAll: return assertresult_ignore;
				}
			#endif
			return assertresult_no;
		}

		void Trace(string map, const BxArgument& args)
		{
			#ifdef ANDROID
				__android_log_print(7,  "SystemJNI", "%s", BxCore::System::
					GetUTF8ByCP949(BxCore::Util::Print(BxCore::System::GetCP949ByUTF8(map), args)));
			#else
				printf("%s\n", BxCore::System::GetUTF8ByCP949(BxCore::Util::Print(map, args)));
				fflush(stdout);
			#endif
		}

		string _tmp_ GetOSName()
		{
			#ifdef WIN32
				return "WINDOWS";
            #elif defined(Q_OS_MACX)
                return "OSX";
			#elif ANDROID
				return "ANDROID";
            #elif TARGET_OS_IPHONE == 1
				return "IOS";
			#else
				return "UNKNOWN";
			#endif
		}

		string _tmp_ GetArchName()
		{
			#ifdef _X86_
				return "X86";
            #elif __ARM_EABI__
				return "ARM";
            #elif defined(Q_OS_MAC)
                return "ARM";
			#else
				return "UNKNOWN";
			#endif
		}

		string _tmp_ GetDeviceID(int* integerid)
		{
			// 맥어드레스ID를 제작
			////////////////////////////////////////
			if(integerid) *integerid = 0;
			return "UNSUPPORTED";
		}

		string _tmp_ GetPhoneNumber()
		{
			////////////////////////////////////////
			return "UNSUPPORTED";
		}

		string _tmp_ GetAppPackage()
		{
            return GetConfigString("Bx.Application.Package", "com.tgr.game");
		}

		string _tmp_ GetAppPackageExt()
		{
			////////////////////////////////////////
			return ".apk";
		}

		string _tmp_ GetAppVersion()
		{
			////////////////////////////////////////
			return GetConfigString("Bx.Application.Version", "1.x.x");
		}

		bool IsExistApp(string packagename)
		{
			////////////////////////////////////////
			return false;
		}

		void PopupOSExecute(string url, bool exitme)
		{
			////////////////////////////////////////
		}

		bool PopupOSKeyboard(string_rw result_str256, string default_str, string keyboard_name, oskeyboardtype type)
		{
			////////////////////////////////////////
			return false;
		}

		bool IsExistClassJNI(string classpath)
		{
			////////////////////////////////////////
			return false;
		}

		int CallStaticMethodJNI(string classpath, string methodname, string param1_str1024, int param2)
		{
			////////////////////////////////////////
			return -90001;
		}

		int AddCallbackJNI(string classpath, string methodname, callback_jni methodCB)
		{
			////////////////////////////////////////
			return -90001;
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Surface
	{
		void Init()
		{
			////////////////////////////////////////
		}

		void Flush(void* ptr, const int row, const int height, const int pitch)
		{
            SurfaceSingle().SetRenderMode(rendermode_null);
            BxCore::OpenGL2D::OpenGLSingle().SwapBuffer();
        }

		void MiddleFlush()
		{
            BxCore::OpenGL2D::OpenGLSingle().Flush();
		}

		int GetWidthHW(bool usemargin)
		{
            const int WidthHW = SurfaceSingle().GetWidthHW();
            if(usemargin && SurfaceSingle().GetMarginEnable())
                return WidthHW - BxCore::Main::GetGUIMarginL() - BxCore::Main::GetGUIMarginR();
            return WidthHW;
		}

		int GetHeightHW(bool usemargin)
		{
            const int HeightHW = SurfaceSingle().GetHeightHW();
            if(usemargin && SurfaceSingle().GetMarginEnable())
                return HeightHW - BxCore::Main::GetGUIMarginT() - BxCore::Main::GetGUIMarginB();
            return HeightHW;
		}

		const bool SetMarginMode(bool enable)
		{
			const bool Result = SurfaceSingle().GetMarginEnable();
            SurfaceSingle().SetMarginEnable(enable);
			return Result;
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Sound
	{
		id_sound Create(string filename, bool autoload)
		{
			////////////////////////////////////////
			return (id_sound) nullptr;
		}

		void Release(id_sound sound)
		{
			////////////////////////////////////////
		}

        void PlayMusic(id_sound sound, bool interrupt, const BxArgument& panorama)
		{
			////////////////////////////////////////
		}

		void StopMusic()
		{
			////////////////////////////////////////
		}

		void SetMusicVolume(uint percent)
		{
			////////////////////////////////////////
		}

		void PlayEffect(id_sound sound, bool interrupt)
		{
			////////////////////////////////////////
		}

		void StopEffect()
		{
			////////////////////////////////////////
		}

		void SetEffectVolume(uint percent)
		{
			////////////////////////////////////////
		}

		void PauseAll()
		{
			////////////////////////////////////////
		}

		void ResumeAll()
		{
			////////////////////////////////////////
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Util
	{
		void* Alloc(int size)
		{
			return (void*) new byte[size];
		}

		void* Free(const void* buf)
		{
			delete[] (const byte*) buf;
			return nullptr;
		}

		void MemSet(void* dst, byte value, int length)
		{
			if(dst) memset(dst, value, length);
		}

		void MemCpy(void* dst, const void* src, int length)
		{
			if(dst) memmove(dst, src, length);
		}

		int MemCmp(const void* dst, const void* src, int length)
		{
			return memcmp(dst, src, length);
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace File
	{
		bool IsExist(string filename)
		{
            return FileClass::IsFile(filename);
		}

		id_file Open(string filename, string mode)
		{
			const bool Writable = (BxUtilGlobal::StrCmp(mode, "wb") == same);
            FileClass* File = BxNew_Param(FileClass, filename, Writable);
            if(File->IsSuccess()) return (id_file) File;
			BxDelete(File);
			return nullptr;
		}

		void Close(id_file file)
		{
			BxASSERT("BxCore::File<파일핸들이 nullptr입니다>", file);
            BxDelete_ByType(FileClass, file);
		}

		uint Read(id_file file, void* buffer, uint length, callback_progress progress)
		{
			BxASSERT("BxCore::File<파일핸들이 nullptr입니다>", file);
			if(!progress)
                return (uint) (*((FileClass*) file))->read((char*) buffer, length);
			uint Result = 0;
			for(uint i = 0; i < length; i += 4096)
			{
                Result += (uint) (*((FileClass*) file))->read(((char*) buffer) + i, BxUtilGlobal::Min(length - i, 4096));
				if(progress && !progress(-1, 0))
					return Result;
			}
			return Result;
		}

		uint ReadVariable(id_file file, uhuge* variable, const byte additionkey, callback_progress progress)
		{
			uint Result = 0;
			byte CurBits = 0x80;
			while((CurBits & 0x80) && Read(file, &CurBits, 1, progress))
			{
				CurBits += additionkey;
				*variable |= (CurBits & 0x7F) << (7 * Result);
				++Result;
			}
			return Result;
		}

		string _tmp_ ReadLine(id_file file, string tab)
		{
			static char Temp[4096 + 1];
			for(int i = 0; i < 4096; ++i)
			{
				char OneChar = '\0';
				const huge ReadSize = (*((FileClass*) file))->read(&OneChar, 1);
				if(ReadSize == 0 && i == 0)
					return nullptr;
				else if(OneChar == '\r' || OneChar == '\0')
				{
					Temp[i] = '\0';
					return Temp;
				}
				else if(OneChar == '\t')
				{
					Temp[i] = '\t';
					if(tab)
					for(int t = 0; i < 4096 && tab[t]; ++i, ++t)
						Temp[i] = tab[t];
				}
				else if(OneChar != '\n')
					Temp[i] = OneChar;
				else --i;
			}
			Temp[4096] = '\0';
			return Temp;
		}

		uint Write(id_file file, const void* buffer, uint length, callback_progress progress)
		{
			BxASSERT("BxCore::File<파일핸들이 nullptr입니다>", file);
			if(!progress)
                return (uint) (*((FileClass*) file))->write((const char*) buffer, length);
			uint Result = 0;
			for(uint i = 0; i < length; i += 4096)
			{
                Result += (uint) (*((FileClass*) file))->write(((const char*) buffer) + i, BxUtilGlobal::Min(length - i, 4096));
				if(progress && !progress(-1, 0))
					return Result;
			}
			return Result;
		}

		uint WriteVariable(id_file file, const uhuge variable, const byte additionkey, callback_progress progress)
		{
			uint Result = 0;
			uhuge AllBits = variable;
			do
			{
				byte CurBits = (AllBits & 0x7F);
				if(AllBits >>= 7) CurBits |= 0x80;
				CurBits += 0x100 - additionkey;
				Write(file, &CurBits, 1, progress);
				++Result;
			}
			while(AllBits);
			return Result;
		}

		uint WriteString(id_file file, string buffer, callback_progress progress)
		{
			return Write(file, buffer, BxUtilGlobal::StrLen(buffer), progress);
		}

		int Skip(id_file file, uint length)
		{
			BxASSERT("BxCore::File<파일핸들이 nullptr입니다>", file);
            (*((FileClass*) file))->seek(length);
            return (int) (*((FileClass*) file))->pos();
		}

		int GetSize(id_file file)
		{
			BxASSERT("BxCore::File<파일핸들이 nullptr입니다>", file);
            return (int) (*((FileClass*) file))->size();
		}

		int GetSize(string filename)
		{
            return (int) FileInfoClass(filename)->size();
		}

		uhuge GetDate(string filename)
		{
            return (uhuge) FileInfoClass(filename)->lastModified().toMSecsSinceEpoch();
		}

		bool Attach(string archivename)
		{
			return false;
		}

		void Detach()
		{
		}

		void MakeDirectory(string dirname, bool isFile)
		{
			char PathName[1024];
			PathName[0] = '\0';
			for(int i = 0; dirname[i]; ++i)
			{
				if(dirname[i] == '/' || dirname[i] == '\\')
				{
                    if(*PathName) DirClass::Mkdir(PathName);
					PathName[i] = '/';
				}
				else PathName[i] = dirname[i];
				PathName[i + 1] = '\0';
			}
			if(*PathName && !isFile)
                DirClass::Mkdir(PathName);
		}

		uint SearchFiles(string dirname, callback_filesearch resultCB, void* data1, void* data2)
		{
			QStringList FileNames;
			DirClass::EntryList(dirname, FileNames);
			uint SearchCount = 0;
            if(0 < FileNames.length())
            {
                char FileName[1024];
                const int DirLength = BxUtilGlobal::StrLen(dirname);
                BxUtilGlobal::StrCpy(FileName, dirname, DirLength);
                FileName[DirLength] = '/';
                for(int i = 0, iend = FileNames.length(); i < iend; ++i)
                {
                    ++SearchCount;
                    if(resultCB)
                    {
                        BxUtilGlobal::StrCpy(&FileName[DirLength + 1],
                            (string) FileNames.at(i).toLocal8Bit().constData());
                        resultCB(!IsExist(FileName), FileName, data1, data2);
                    }
                }
            }
			return SearchCount;
		}

		bool RemoveFile(string filename, bool doRemoveBlankedDirectory)
		{
            if(!FileClass::Remove(filename))
				return false;
			else if(doRemoveBlankedDirectory)
			{
				BxString::Parse DirName(filename);
				do
				{
					DirName.DeleteLast();
					while(DirName.GetLast() != '/' && DirName.GetLast() != '\\')
						DirName.DeleteLast();
				}
                while(DirClass::Rmdir((string) DirName));
			}
			return true;
		}

		bool RenameFile(string srcname, string dstname)
		{
            return FileClass::Rename(srcname, dstname);
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Socket
	{
		id_socket Create()
		{
			TCPData* TCP = BxNew(TCPData);
			if(TCP->GetState() == socketstate_created)
				return (id_socket) TCP;
			BxDelete(TCP);
			return nullptr;
		}

		void Release(id_socket sock)
		{
			if(!sock) return;
			BxDelete_ByType(TCPData, sock);
		}

		socketstate GetState(id_socket sock)
		{
			if(!sock) return socketstate_null;
			return ((TCPData*) sock)->GetState();
		}

		connectresult Connect(id_socket sock, string addr, ushort port, uint timeout, callback_progress progress)
		{
			if(!sock) return connect_error_param;
			TCPData* TCP = (TCPData*) sock;
			if(TCP->GetState() == socketstate_null) return connect_error_param;
			else if(TCP->GetState() == socketstate_connecting) return connect_connecting;
			else if(TCP->GetState() == socketstate_connected) return connect_connected;
			// IP조사
			QList<QHostAddress> Address = QHostInfo::fromName(QString(addr)).addresses();
			if(Address.length() == 0) return connect_error_wrong_address;
			TCP->SetState(socketstate_connecting);
			TCP->connectToHost(Address[0], port);
			if(timeout == 0) return connect_connecting;
			// 결과확인
			if(!TCP->waitForConnected(timeout))
			{
				TCP->Disconnect(true);
				return connect_disconnected;
			}
			return connect_connected;
		}

		void Disconnect(id_socket sock)
		{
			if(!sock) return;
			TCPData* TCP = (TCPData*) sock;
			TCP->Disconnect(false);
		}

		int Send(id_socket sock, const void* buffer, int len)
		{
			if(!sock) return -1;
			TCPData* TCP = (TCPData*) sock;
			if(TCP->GetState() != socketstate_connected)
				return -2;
			const int Result = TCP->write((const char*) buffer, len);
			TCP->flush();
			if(0 <= Result) return Result;
			return -100 - ((int) TCP->error());
		}

		int Recv(id_socket sock, void* buffer, int len)
		{
			if(!sock) return -1;
			TCPData* TCP = (TCPData*) sock;
			if(TCP->GetState() != socketstate_connected)
				return -2;
			const int Result = TCP->read((char*) buffer, len);
			if(0 <= Result) return Result;
			return -100 - ((int) TCP->error());
		}

		int Ping(string addr, uint timeout)
		{
			return -1;
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Font
	{
		id_font Open(string nick, int height)
		{
			return (id_font) FontSingle().CreateFont(nick, height);
		}

		id_font_zero Close(id_font font)
		{
			if(font) FontSingle().RemoveFont((FontClass*) font);
			return nullptr;
		}

		bool NickOpen(string nick, string filename)
		{
			id_file FontFile = BxCore::File::Open(filename, "rb");
			BxASSERT("BxCore::Font<해당 파일에 접근할 수 없습니다>", FontFile);
			if(!FontFile) return false;
			const int FontBufferSize = BxCore::File::GetSize(FontFile);
			byte* FontBuffer = (byte*) BxAlloc(FontBufferSize);
			BxCore::File::Read(FontFile, FontBuffer, FontBufferSize);
			BxCore::File::Close(FontFile);
			return NickOpenByBuffer(nick, FontBuffer, FontBufferSize);
		}

		bool NickOpenByBuffer(string nick, const byte* buffer, int buffersize)
		{
			return FontSingle().CreateGraph(nick, buffer, buffersize);
		}

		void NickClose(string nick)
		{
			FontSingle().RemoveGraph(nick);
		}

		bool IsExistNick(string nick)
		{
			return FontSingle().IsExistGraph(nick);
		}

		id_text TextOpen(string str, textencoding encoding)
		{
			return (id_text) FontSingle().CreateText(str, encoding);
		}

		id_text TextOpenByUTF16(wstring wstr, bool referonly)
		{
			return (id_text) FontSingle().CreateText(wstr, referonly);
		}

		id_text_zero TextClose(id_text text)
		{
			if(text) FontSingle().RemoveText((TextClass*) text);
			return nullptr;
		}

		void SetOption(id_font font, color_x888 color, byte opacity, textalign align)
		{
			FontClass* Font = (FontClass*) font;
			BxASSERT("BxCore::Font<Font가 nullptr입니다>", Font);
			Font->SetOption(color, opacity, align);
		}

		void SetRotate(id_font font, int angle1024)
		{
			FontClass* Font = (FontClass*) font;
			BxASSERT("BxCore::Font<Font가 nullptr입니다>", Font);
			Font->SetRotate(angle1024);
		}

		void SetJustify(id_font font, int column)
		{
			FontClass* Font = (FontClass*) font;
			BxASSERT("BxCore::Font<Font가 nullptr입니다>", Font);
			Font->SetJustify(column);
		}

		int GetWidth(id_font font, id_text text, int offset, int length)
		{
			FontClass* Font = (FontClass*) font;
			TextClass* Text = (TextClass*) text;
			BxASSERT("BxCore::Font<Font가 nullptr입니다>", Font);
			BxASSERT("BxCore::Font<Text가 nullptr입니다>", Text);
			if(length == -1) length = Text->GetLength() - offset;
			return Font->GetWidth(*Text, offset, length);
		}

		int GetHeight(id_font font)
		{
			FontClass* Font = (FontClass*) font;
			BxASSERT("BxCore::Font<Font가 nullptr입니다>", Font);
			return Font->GetHeight();
		}

		int GetLength(id_font font, int column, id_text text, int offset, int* width)
		{
			FontClass* Font = (FontClass*) font;
			TextClass* Text = (TextClass*) text;
			BxASSERT("BxCore::Font<Font가 nullptr입니다>", Font);
			BxASSERT("BxCore::Font<Text가 nullptr입니다>", Text);
			return Font->GetLength(column, *Text, offset, width);
		}

		int GetLengthByLetter(id_font font, int column, id_text text, int offset, int* width)
		{
			FontClass* Font = (FontClass*) font;
			TextClass* Text = (TextClass*) text;
			BxASSERT("BxCore::Font<Font가 nullptr입니다>", Font);
			BxASSERT("BxCore::Font<Text가 nullptr입니다>", Text);
			return Font->GetLengthByLetter(column, *Text, offset, width);
		}

		point DrawRequest(id_font font, point pos, id_text text, int offset, int length)
		{
			FontClass* Font = (FontClass*) font;
			TextClass* Text = (TextClass*) text;
			BxASSERT("BxCore::Font<Font가 nullptr입니다>", Font);
			BxASSERT("BxCore::Font<Text가 nullptr입니다>", Text);
			if(length == -1) length = Text->GetLength() - offset;
			return Font->DrawRequest(pos, *Text, offset, length);
		}

		void RequestFlush(id_font font)
		{
			FontClass* Font = (FontClass*) font;
			BxASSERT("BxCore::Font<Font가 nullptr입니다>", Font);
			if(BxCore::System::IsEnableFontDebug())
				Font->CheckRequestDebug(FontSingle().GetFocusedProgramDebug());
			Font->RequestFlush(FontSingle().GetFocusedProgram());
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Thread
	{
		id_thread Create(callback_thread threadCB, void* data)
		{
			ThreadClass* Class = new ThreadClass(threadCB, data);
			return (id_thread) Class;
		}

		void Release(id_thread thread, bool dokill, bool dowait)
		{
			if(!thread) return;
			ThreadClass* Class = (ThreadClass*) thread;
			if(dokill) Class->exit();
			if(dowait) Class->wait();
			delete Class;
		}

		id_mutex OpenMutex()
		{
			QMutex* Mutex = new QMutex();
			return (id_mutex) Mutex;
		}

		void CloseMutex(id_mutex mutex)
		{
			if(!mutex) return;
			delete (QMutex*) mutex;
		}

		void Lock(id_mutex mutex)
		{
			if(!mutex) return;
			((QMutex*) mutex)->lock();
		}

		void Unlock(id_mutex mutex)
		{
			if(!mutex) return;
			((QMutex*) mutex)->unlock();
		}

		void* BindStorage(int* storagekey)
		{
			// 스레드별 저장소그룹 접근
			Lock(StorageClass::Mutex());
			StorageClass::VarMap& StorageByThread = StorageClass::Map()[StorageClass::CurrentThreadID()];
			Unlock(StorageClass::Mutex());
			// 스레드내 개별저장소 접근
            StorageClass* Result = StorageByThread.Access((mint) storagekey);
			if(Result) return Result->Data;
            return StorageByThread[(mint) storagekey].Init(*storagekey);
		}

		void UnbindStorageAll()
		{
			Lock(StorageClass::Mutex());
			StorageClass::Map()[StorageClass::CurrentThreadID()].Reset();
			Unlock(StorageClass::Mutex());
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Library
	{
		id_library Open(string filename)
		{
			#ifdef WIN32
				QLibrary* Library = new QLibrary(filename);
				if(Library->load()) return (id_library) Library;
				delete Library;
				return nullptr;
			#else
				return nullptr;
			#endif
		}

		void Close(id_library handle)
		{
			#ifdef WIN32
				delete (QLibrary*) handle;
			#endif
		}

		void* Link(id_library handle, string name)
		{
			#ifdef WIN32
				return (void*) ((QLibrary*) handle)->resolve(name);
			#else
				return nullptr;
			#endif
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace OpenGL2D
	{
		void Init()
		{
            ////////////////////////////////////////
		}

		void Quit()
		{
            ////////////////////////////////////////
		}

		void Flush()
		{
            OpenGLSingle().Flush();
		}

		void SetPerspective(const int mul, const int far, const int near)
		{
			////////////////////////////////////////
		}

		void SetOrthogonal(const int width, const int height, const int far, const int near)
		{
			////////////////////////////////////////
		}

		void AddViewTrans(int x, int y, int z)
		{
			////////////////////////////////////////
		}

		void AddViewRotX(fint angle)
		{
			////////////////////////////////////////
		}

		void AddViewRotY(fint angle)
		{
			////////////////////////////////////////
		}

		void AddViewRotZ(fint angle)
		{
			////////////////////////////////////////
		}

		id_opengl_form MakeForm_TextureMaterial(string filename)
		{
			TextureMaterial* Result = new TextureMaterial();
			glGenTextures(1, &Result->TextureID);
            BxCore::Main::GLWidget::Me().BindTexture(Result->TextureID);
			QImage NewImage(filename);
			NewImage = QGLWidget::convertToGLFormat(NewImage); // 상하반전 및 기타변환
			const int width = NewImage.width();
			const int height = NewImage.height();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NewImage.bits());
			Result->UVRate.x = (1 << 11) * (width - GetTextureMargin() * 2) / width;
			Result->UVRate.y = (1 << 11) * (height - GetTextureMargin() * 2) / height;
            return (id_opengl_form) Result;
		}

		id_opengl_form MakeForm_TextureMaterial(color_a888* image, const int width, const int height, const int pitch)
		{
			TextureMaterial* Result = new TextureMaterial(); // <!> 자체메모리로 하면 침범발생
			glGenTextures(1, &Result->TextureID);
            BxCore::Main::GLWidget::Me().BindTexture(Result->TextureID);
			QImage NewImage((uchar*) image, width, height, sizeof(color_a888) * pitch, QImage::Format_ARGB32);
			NewImage = QGLWidget::convertToGLFormat(NewImage); // 상하반전 및 기타변환
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NewImage.bits());
			Result->UVRate.x = (1 << 11) * (width - GetTextureMargin() * 2) / width;
			Result->UVRate.y = (1 << 11) * (height - GetTextureMargin() * 2) / height;
            return (id_opengl_form) Result;
		}

		id_opengl_form MakeForm_ColorAmbient(const color_x888 color)
		{
            static ColorAmbient ResultData;
            ColorAmbient* Result = &ResultData;
            Result->Color = color;
            return (id_opengl_form) Result;
		}

		void FreeForm(id_opengl_form form)
		{
            OpenGLForm* Form = (OpenGLForm*) form;
            if(Form && Form->Type != OpenGLForm::COLOR)
				delete Form;
		}

		id_opengl_outline MakeOutline_VertexUV(const rect& r, const rect& uvclip)
		{
            static VertexUV ResultData;
            VertexUV* Result = &ResultData;
            // Vertex
			Result->Vertex[0].x = ItoF(r.l);
			Result->Vertex[0].y = ItoF(r.t);
			Result->Vertex[1].x = ItoF(r.l);
			Result->Vertex[1].y = ItoF(r.b);
			Result->Vertex[2].x = ItoF(r.r);
			Result->Vertex[2].y = ItoF(r.b);
			Result->Vertex[3].x = ItoF(r.r);
			Result->Vertex[3].y = ItoF(r.t);
            // UV
			Result->UV[0].x = uvclip.l; Result->UV[0].y = uvclip.t;
			Result->UV[1].x = uvclip.l; Result->UV[1].y = uvclip.b;
			Result->UV[2].x = uvclip.r; Result->UV[2].y = uvclip.b;
			Result->UV[3].x = uvclip.r; Result->UV[3].y = uvclip.t;
            return (id_opengl_outline) Result;
		}

		id_opengl_outline MakeOutline_PolyVertexUV(const points& p, const rect& uvclip)
		{
			PolyVertexUV* Result = BxNew_Param(PolyVertexUV, p.count);
			BxASSERT("BxCore::OpenGL2D<파라미터 p의 count는 3 이상이어야 합니다>", 3 <= p.count);
			// Vertex
			rectf Bound = {0, 0, 0, 0};
			const bool IsClockwise = (BxUtil::GetClockwiseValue(p.pt[0], p.pt[1], p.pt[2]) < 0);
			for(int i = 0; i < p.count; ++i)
			{
				const int j = (IsClockwise)? (p.count - i) % p.count : i;
				Result->Vertex[i].x = ItoF(p.pt[j].x);
				Result->Vertex[i].y = ItoF(p.pt[j].y);
				Bound.l = (i == 0)? Result->Vertex[i].x : BxUtilGlobal::Min(Bound.l, Result->Vertex[i].x);
				Bound.t = (i == 0)? Result->Vertex[i].y : BxUtilGlobal::Min(Bound.t, Result->Vertex[i].y);
				Bound.r = (i == 0)? Result->Vertex[i].x : BxUtilGlobal::Max(Bound.r, Result->Vertex[i].x);
				Bound.b = (i == 0)? Result->Vertex[i].y : BxUtilGlobal::Max(Bound.b, Result->Vertex[i].y);
			}
			if(Bound.r <= Bound.l || Bound.b <= Bound.t)
			{
				BxDelete(Result);
				return nullptr;
			}
			// UV
			BxExpress::IntegerX BoundWidth(Bound.r - Bound.l + BxInteger(0));
			BxExpress::IntegerX BoundHeight(Bound.b - Bound.t + BxInteger(0));
			for(int i = 0; i < p.count; ++i)
				Result->UV[i] = XY(
					uvclip.l + (short)((uvclip.r - uvclip.l) * (huge)(Result->Vertex[i].x - Bound.l) / BoundWidth),
					uvclip.t + (short)((uvclip.b - uvclip.t) * (huge)(Result->Vertex[i].y - Bound.t) / BoundHeight));
			return (id_opengl_outline) Result;
		}

		id_opengl_outline MakeOutline_PolyVertexUV3D(const vertexs& v, const points& uv)
		{
			////////////////////////////////////////
			return nullptr;
		}

		id_opengl_outline MakeOutline_SplineVertex(const spline& s)
		{
			SplineVertex* Result = BxNew_Param(SplineVertex);
			// Vertex
			for(int i = 0; i < 4; ++i)
			{
				Result->VertexL[i].x = s.lx[i];
				Result->VertexL[i].y = s.ly[i];
				Result->VertexR[i].x = s.rx[i];
				Result->VertexR[i].y = s.ry[i];
			}
			return (id_opengl_outline) Result;
		}

		void FreeOutline(id_opengl_outline outline)
		{
            OpenGLOutline* Outline = (OpenGLOutline*) outline;
            if(Outline && Outline->Type != OpenGLOutline::RECT)
                BxDelete(Outline);
		}

		color_x888 Form_ColorAmbient_GetColor(id_opengl_form form)
		{
            OpenGLForm* Form = (OpenGLForm*) form;
            BxASSERT("BxCore::OpenGL2D<파라미터가 ColorAmbient형이 아닙니다>", Form->Type == OpenGLForm::COLOR);
            return ((ColorAmbient*) Form)->Color;
		}

		void Outline_VertexUV_SetScale(id_opengl_outline outline, fint hoz, fint ver)
		{
            OpenGLOutline* Outline = (OpenGLOutline*) outline;
            BxASSERT("BxCore::OpenGL2D<파라미터가 VertexUV형이 아닙니다>", Outline->Type == OpenGLOutline::RECT);
            if(hoz == 0x10000 && ver == 0x10000) return;
            VertexUV* VUV = (VertexUV*) Outline;
            for(int i = 0; i < 4; ++i)
            {
                VUV->Vertex[i].x = (fint)((VUV->Vertex[i].x * (huge) hoz + 0x8000) >> 16);
                VUV->Vertex[i].y = (fint)((VUV->Vertex[i].y * (huge) ver + 0x8000) >> 16);
            }
		}

		void Outline_VertexUV_SetRotate(id_opengl_outline outline, int angle1024, bool doFlip)
		{
            OpenGLOutline* Outline = (OpenGLOutline*) outline;
            BxASSERT("BxCore::OpenGL2D<파라미터가 VertexUV형이 아닙니다>", Outline->Type == OpenGLOutline::RECT);
            if(angle1024 == 0 && !doFlip) return;
            VertexUV* VUV = (VertexUV*) Outline;
            const huge Cos = BxUtil::Cos(angle1024);
            const huge Sin = BxUtil::Sin(angle1024);
            fint MinX = 0, MaxX = 0;
            if(doFlip)
            for(int i = 0; i < 4; ++i)
            {
                MinX = (i == 0)? VUV->Vertex[i].x : BxUtilGlobal::Min(MinX, VUV->Vertex[i].x);
                MaxX = (i == 0)? VUV->Vertex[i].x : BxUtilGlobal::Max(MaxX, VUV->Vertex[i].x);
            }
            for(int i = 0; i < 4; ++i)
            {
                const fint TempX = (doFlip)? MinX + MaxX - VUV->Vertex[i].x : VUV->Vertex[i].x;
                VUV->Vertex[i].x = (fint)((TempX * Cos - VUV->Vertex[i].y * Sin + 0x8000) >> 16);
                VUV->Vertex[i].y = (fint)((TempX * Sin + VUV->Vertex[i].y * Cos + 0x8000) >> 16);
            }
		}

		void Outline_PolyVertexUV_SetScale(id_opengl_outline outline, fint hoz, fint ver)
		{
			OpenGLOutline* Outline = (OpenGLOutline*) outline;
			if(hoz == 0x10000 && ver == 0x10000) return;

			if(Outline->Type == OpenGLOutline::RECT)
			{
				VertexUV* VUV = (VertexUV*) Outline;
				for(int i = 0; i < 4; ++i)
				{
					VUV->Vertex[i].x = (fint)((VUV->Vertex[i].x * (huge) hoz + 0x8000) >> 16);
					VUV->Vertex[i].y = (fint)((VUV->Vertex[i].y * (huge) ver + 0x8000) >> 16);
				}
			}
			else if(Outline->Type == OpenGLOutline::POLYGON)
			{
				PolyVertexUV* PVUV = (PolyVertexUV*) Outline;
				for(int i = 0; i < PVUV->Count; ++i)
				{
					PVUV->Vertex[i].x = (fint)((PVUV->Vertex[i].x * (huge) hoz + 0x8000) >> 16);
					PVUV->Vertex[i].y = (fint)((PVUV->Vertex[i].y * (huge) ver + 0x8000) >> 16);
				}
			}
		}

		void Outline_PolyVertexUV_SetRotate(id_opengl_outline outline, int angle1024, bool doFlip)
		{
			OpenGLOutline* Outline = (OpenGLOutline*) outline;
			if(angle1024 == 0 && !doFlip) return;

			const huge Cos = BxUtil::Cos(angle1024);
			const huge Sin = BxUtil::Sin(angle1024);
			fint MinX = 0, MaxX = 0;

			if(Outline->Type == OpenGLOutline::RECT)
			{
				VertexUV* VUV = (VertexUV*) Outline;
				if(doFlip)
				for(int i = 0; i < 4; ++i)
				{
					MinX = (i == 0)? VUV->Vertex[i].x : BxUtilGlobal::Min(MinX, VUV->Vertex[i].x);
					MaxX = (i == 0)? VUV->Vertex[i].x : BxUtilGlobal::Max(MaxX, VUV->Vertex[i].x);
				}
				for(int i = 0; i < 4; ++i)
				{
					const fint TempX = (doFlip)? MinX + MaxX - VUV->Vertex[i].x : VUV->Vertex[i].x;
					VUV->Vertex[i].x = (fint)((TempX * Cos - VUV->Vertex[i].y * Sin + 0x8000) >> 16);
					VUV->Vertex[i].y = (fint)((TempX * Sin + VUV->Vertex[i].y * Cos + 0x8000) >> 16);
				}
			}
			else if(Outline->Type == OpenGLOutline::POLYGON)
			{
				PolyVertexUV* PVUV = (PolyVertexUV*) Outline;
				if(doFlip)
				for(int i = 0; i < PVUV->Count; ++i)
				{
					MinX = (i == 0)? PVUV->Vertex[i].x : BxUtilGlobal::Min(MinX, PVUV->Vertex[i].x);
					MaxX = (i == 0)? PVUV->Vertex[i].x : BxUtilGlobal::Max(MaxX, PVUV->Vertex[i].x);
				}
				for(int i = 0; i < PVUV->Count; ++i)
				{
					const fint TempX = (doFlip)? MinX + MaxX - PVUV->Vertex[i].x : PVUV->Vertex[i].x;
					PVUV->Vertex[i].x = (fint)((TempX * Cos - PVUV->Vertex[i].y * Sin + 0x8000) >> 16);
					PVUV->Vertex[i].y = (fint)((TempX * Sin + PVUV->Vertex[i].y * Cos + 0x8000) >> 16);
				}
			}
		}

		void Render(id_opengl_form form, id_opengl_outline outline, int x, int y, const byte opacity, const color_x888 color)
		{
            OpenGLSingle().BindForm((OpenGLForm*) form);
            OpenGLSingle().DrawOutline((OpenGLOutline*) outline, ((OpenGLForm*) form)->UVRate,
                x, y, opacity, color);
		}

		void Render3D(id_opengl_form form, id_opengl_outline outline, int x, int y, int z, const byte opacity, const color_x888 color)
		{
			////////////////////////////////////////
		}

		void Clip(rect r)
		{
            if(0 < r.r - r.l && 0 < r.b - r.t)
                OpenGLSingle().SetScissor(r.l, r.t, r.r - r.l, r.b - r.t);
		}
	}
}
