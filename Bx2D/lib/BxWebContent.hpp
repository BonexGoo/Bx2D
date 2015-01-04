#pragma once
#include <BxString.hpp>
#include <BxVarMap.hpp>
#include <BxVarVector.hpp>

//! \brief HTTP기반 쿼리송수신, 파일다운로드
class BxWebContent
{
	private: class ContactPool
	{
		public: class Cookie
		{
			public: string_rw Name;
			public: string_rw Value;
			public: Cookie() : Name(nullptr), Value(nullptr) {}
			public: ~Cookie() {Name = BxUtilGlobal::StrFree(Name); Value = BxUtilGlobal::StrFree(Value);}
			public: Cookie* Init(string NewName, string NewValue)
			{
				Name = BxUtilGlobal::StrFree(Name);
				Name = BxUtilGlobal::StrCpyWithAlloc(NewName);
				SetValue(NewValue);
				return this;
			}
			public: void SetValue(string NewValue)
			{
				Value = BxUtilGlobal::StrFree(Value);
				Value = BxUtilGlobal::StrCpyWithAlloc(NewValue);
			}
		};
		public: BxVarMap<Cookie*> CookieMap;
		public: BxVarVector<Cookie, 4> CookieList;
		public: id_socket Socket;
		public: ContactPool() : Socket(nullptr) {}
		public: ~ContactPool() {BxCore::Socket::Release(Socket);}
		private: typedef BxVarMap<ContactPool> ContactMap;
		private: global_func BxVarMap<ContactMap>& Pool()
		{global_data BxVarMap<ContactMap>* _ = BxNew(BxVarMap<ContactMap>); return *_;}
		public: global_func ContactPool* Access(string Domain, ushort Port) {return &Pool()(Domain)[Port];}
		public: global_func void Remove(string Domain, ushort Port)
		{
			if(!Pool().Access(Domain)) return;
			Pool()(Domain).Remove(Port);
			if(0 < Pool()(Domain).Count()) return;
			Pool().Remove(Domain);
		}
	};

	private: int ResponseLength;
	private: byte* ResponseBytes;
	private: ContactPool* _ref_ LastContact;
	private: BxString LastError;

	private: class CachePool
	{
		public: string_rw Path;
		public: uint Size;
		public: uhuge Date;
		public: CachePool() : Path(nullptr), Size(0), Date(0) {}
		public: ~CachePool() {Path = BxUtilGlobal::StrFree(Path);}
	};
	private: typedef BxVarVector<CachePool, 64> CachePoolList;

	private: global_func macro bool& IsInitPool() {global_data bool _IsInitPool = false; return _IsInitPool;}
	private: global_func macro CachePoolList& Pool() {global_data CachePoolList* _ = BxNew(CachePoolList); return *_;}
	private: global_func macro int& PoolSumSize() {global_data int _PoolSumSize = 0; return _PoolSumSize;}
	private: global_func void OnFileSearch(bool isdirname, string _tmp_ name, void* data1, void* data2)
	{
		BxString Name(name); // 안전을 위해 재귀처리는 새로운 공간할당이 필요
		if(!isdirname)
		{
			if(Name.GetFileExt().CompareNoCase("downloading") != same)
			{
				Pool()[LAST].Path = Name.CloneWithAlloc();
				PoolSumSize() += (Pool()[END].Size = BxCore::File::GetSize(Name));
				Pool()[END].Date = BxCore::File::GetDate(Name);
			}
		}
		else BxCore::File::SearchFiles(Name, OnFileSearch, data1, data2);
	}

	public: BxWebContent()
	{
		ResponseLength = 0;
		ResponseBytes = nullptr;
		LastContact = nullptr;
		LastError = "";
		ResponseJSON = nullptr;
	}
	public: ~BxWebContent()
	{
		BxFree(ResponseBytes);
		BxDelete(ResponseJSON);
	}

	/*!
	\brief 캐시폴더이름 받기
	\return 캐시폴더이름
	*/
	public: global_func inline string GetCachePath() {return ".webcontent_cache";}

	/*!
	\brief 쿼리결과의 사이즈얻기(데이터)
	\return 길이(byte)
	*/
	public: inline int GetDataLength() {return ResponseLength;}

	/*!
	\brief 쿼리결과의 사이즈얻기(스트링)
	\return 길이(byte)
	*/
	public: inline int GetTextLength() {return BxUtilGlobal::StrLenEx((string) ResponseBytes);}

	/*!
	\brief 쿼리결과 받기
	\return 쿼리결과 바이트배열
	*/
	public: inline const byte* _tmp_ GetBytes() {return ResponseBytes;}

	//! \brief JSON파서
	public: class JSON
	{
		/*!
		\brief 자식접근
		\param ChildName : 자식의 이름
		\return 해당 자식의 인스턴스
		*/
		public: JSON& operator()(string ChildName) const
		{
			JSON* Result = Child.Access(ChildName);
			return (Result)? *Result : NullValue();
		}

		/*!
		\brief 배열식 자식접근
		\param ChildIndex : 자식의 번호
		\return 해당 자식의 인스턴스
		*/
		public: JSON& operator[](int ChildIndex) const
		{
			JSON* Result = Child.Access(ChildIndex);
			return (Result)? *Result : NullValue();
		}

		/*!
		\brief 수량정보
		\return 자식의 수량
		*/
		public: int Length() const {return Child.Count();}

		/*!
		\brief 정수식 값정보
		\return 자신의 정수값
		*/
		public: int ToInt() const {return BxUtilGlobal::AtoI(String, String.GetLength());}

		/*!
		\brief 스트링식 값정보
		\return 자신의 스트링값
		*/
		public: string ToStr() const {return String;}

		private: class ParseStack
		{
		public: int Index;
		public: JSON* Object;
		public: ParseStack() : Index(-1), Object(nullptr) {}
		public: ~ParseStack() {}
		};

		/*!
		\brief 스크립트 파싱
		\param Script : 파싱할 스크립트
		*/
		public: void Parse(string Script)
		{
			Child.Reset();
			String = "";
			while(*Script != '{')
			{
				if(*Script & 0x80) Script += 2; else ++Script;
				if(*Script == '\0')
					return; // 에러페이지
			}
			BxVarVector<ParseStack, 4> CurStack;
			CurStack[0].Object = nullptr;
			CurStack[1].Object = this;
			BxString::Parse LastString("");
			bool IsCollectString = true;
			do switch(*Script)
			{
			case ':':
				{
					JSON* NewChild = &CurStack[END].Object->Child((string) LastString);
					CurStack[LAST].Object = NewChild;
					// 주의!) 아래와 같은 코드에서 GCC는 좌항의 배열첨자를 먼저 실행시킬 수 있음
					// CurStack[LAST].Object = &CurStack[END].Object->Child((string) LastString);
				}
				LastString = "";
				break;
			case '[':
				if(Script[1] != ']')
				{
					CurStack[END].Index = 0;
					JSON* NewChild = &CurStack[END].Object->Child[CurStack[END].Index++];
					CurStack[LAST].Object = NewChild;
				}
				else ++Script;
				LastString = "";
				break;
			case ',': case '}': case ']':
				CurStack[END].Object->String = LastString;
				CurStack.Delete(END);
				BxASSERT("BxWebContent<잘못된 JSON스크립트입니다>", 2 <= CurStack.Length());
				if(*Script == ',' && -1 < CurStack[END].Index)
				{
					JSON* NewChild = &CurStack[END].Object->Child[CurStack[END].Index++];
					CurStack[LAST].Object = NewChild;
				}
				else if(*Script == '}' && CurStack.Length() == 2)
					return; // JSON완료
				LastString = "";
				IsCollectString = false;
				break;
			case ' ': case '\t': case '\r': case '\n':
				IsCollectString = false;
				break;
			// 스트링수집
			case '\"': case '\'':
				{
					LastString = "";
					const char EndMark = *Script;
					while(*(++Script) && *Script != EndMark)
					{
						if(*Script & 0x80)
							LastString += *(Script++);
						LastString += *Script;
					}
					if(!*Script) Script--;
				}
				break;
			default:
				if(!IsCollectString)
				{
					IsCollectString = true;
					LastString = "";
				}
				if(Script[0] != '\\' || Script[1] == '\0')
				{
					if(*Script & 0x80)
						LastString += *(Script++);
					LastString += *Script;
					
				}
				else switch(*(++Script))
				{
				case '\\': LastString += '\\'; break;
				case 't': LastString += '\t'; break;
				case 'r': LastString += '\r'; break;
				case 'n': LastString += '\n'; break;
				case '\"': LastString += '\"'; break;
				case '\'': LastString += '\''; break;
				}
				break;
			} while(*(++Script) != '\0');
		}

		/*!
		\brief 생성자, 소멸자
		*/
		public: JSON() {}
		public: ~JSON() {}
		private: BxVarMap<JSON> Child;
		private: BxString::Parse String;
		private: global_func JSON& NullValue() {global_data JSON* _ = BxNew(JSON); return *_;}
	} *ResponseJSON;

	/*!
	\brief 쿼리결과 JSON으로 받기
	\param ChildName : 첫번째 자식의 이름
	\return 해당 자식의 JSON객체 인스턴스
	*/
	public: inline const JSON& GetJSON(string ChildName)
	{
		if(!ResponseJSON)
		{
			ResponseJSON = BxNew(JSON);
			ResponseJSON->Parse(BxCore::System::GetCP949ByUTF8((string) ResponseBytes));
		}
		return (*ResponseJSON)(ChildName);
	}

	/*!
	\brief 마지막 에러내용 받기
	\return 에러내용
	*/
	public: inline string GetLastError() {return LastError;}
	private: inline bool SetLastError(string ErrorMessage)
	{
		ResponseLength = 0;
		BxFree(ResponseBytes);
		BxDelete(ResponseJSON);
		if(LastContact)
		{
			BxCore::Socket::Release(LastContact->Socket);
			LastContact->Socket = nullptr;
			LastContact = nullptr;
		}
		LastError = ErrorMessage;
		return false;
	}

	/*!
	\brief 특정 Contact정보(소켓/쿠키)를 지움
	\param Domain : 접속된 서버
	\param Port : 포트번호
	\see Query
	*/
	public: global_func void ClearContact(string Domain, ushort Port) {ContactPool::Remove(Domain, Port);}

	/*!
	\brief HTTP쿼리실행
	\param Domain : 접속할 서버(예: "www.naver.com")
	\param Port : 포트번호(HTTP는 보통 80번포트)
	\param Path : 다운받을 파일 또는 실행할 서블릿의 경로(예: "Update/Run.aspx")
	\param Arguments : 서블릿의 경우 전달할 인자들(예: "Data0=aaa&Data1=bbb&Data2=ccc")
	\param Timeout : 각 단계별 지연허용시간
	\param Progress : 프로그레스의 전달
	\return 송수신 성공여부
	\see ClearContact, GetLastError
	*/
	public: bool Query(string Domain, ushort Port, string Path, string Arguments = nullptr, int Timeout = -1, callback_progress Progress = nullptr)
	{
		// 필요시 소켓생성 및 연결
		if(Timeout < 0) Timeout = 5000;
		LastContact = ContactPool::Access(Domain, Port);
		if(LastContact->Socket == nullptr || socketstate_connected != BxCore::Socket::GetState(LastContact->Socket))
		{
			BxCore::Socket::Release(LastContact->Socket);
			LastContact->Socket = BxCore::Socket::Create();
			if(connect_connected != BxCore::Socket::Connect(LastContact->Socket, Domain, Port, Timeout, Progress))
				return SetLastError("Failed connect"); // 연결실패
		}

		// 쿠키모음
		BxString::Parse Cookies;
		for(int i = 0, iend = LastContact->CookieList.Length(); i < iend; ++i)
		{
			Cookies += "Cookie: ";
			Cookies += LastContact->CookieList[i].Name;
			Cookies += LastContact->CookieList[i].Value;
			Cookies += "\r\n";
		}
		Cookies += "\r\n";

		// 쿼리제작
		string _tmp_ RequestChars_CP949 = BxCore::Util::Print(
			"<>:GET /<A><A><A> HTTP/1.1<R><N>"
			"Accept: text/html, application/xhtml+xml, */*<R><N>"
			"Accept-Language: ko-KR<R><N>"
			"User-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)<R><N>"
			"Accept-Encoding: deflate<R><N>"
			"Host: <A><R><N>"
			"Connection: Keep-Alive<R><N>"
			"<A>", BxARG(Path, (!Arguments || !*Arguments)? "" : "?", (!Arguments)? "" : Arguments, Domain, Cookies));
		string _tmp_ RequestChars = BxCore::System::GetUTF8ByCP949(RequestChars_CP949);
		int RequestCharsLength = BxUtilGlobal::StrLen(RequestChars);

		// 쿼리전송
		int SendResult = 0, SendedLength = 0;
		do
		{
			if(Progress && !Progress(-1, 0)) return false; // 업데이트
			if((SendResult = BxCore::Socket::Send(LastContact->Socket, RequestChars + SendedLength, RequestCharsLength - SendedLength)) < 0)
				return SetLastError("Failed sending for HTTP request"); // 송신실패
		}
		while((SendedLength += SendResult) < RequestCharsLength);
		BxCore::System::Sleep(0); // 이벤트확인

		// HTTP관련 정적변수
		static bool IsFirstSetting = true;
		static int EndCodeMap[256];
		static int EndLineMap[256];
		static int ContentLengthMap[256];
		static int ConnectionMap[256];
		static int SetCookieMap[256];
		if(IsFirstSetting)
		{
			IsFirstSetting = false;
			BxUtilGlobal::StrSearchMap("\r\n\r\n", EndCodeMap);
			BxUtilGlobal::StrSearchMap("\r\n", EndLineMap);
			BxUtilGlobal::StrSearchMap("Content-Length:", ContentLengthMap);
			BxUtilGlobal::StrSearchMap("Connection: keep-alive", ConnectionMap);
			BxUtilGlobal::StrSearchMap("Set-Cookie:", SetCookieMap);
		}

		// HTTP헤더 수신
		char ResponseChars[4096];
		int RecvResult = 0, RecvedLength = 0;
		int ErrorTime = 0, SleepTime = 0, EndCodePos = -1;
		bool KeepAlive = false;
		while(0 <= (RecvResult = BxCore::Socket::Recv(LastContact->Socket, ResponseChars + RecvedLength, 4096 - RecvedLength)))
		{
			if(Progress && !Progress(-1, 0)) return false; // 업데이트
			RecvedLength += RecvResult;
			// 받은 내용이 있을 경우
			if(0 < RecvResult)
			{
				ErrorTime = 0;
				SleepTime >>= 1;
				// HTML헤더 종료코드 검색
				if(4 < RecvedLength) // "\r\n\r\n"
				{
					EndCodePos = BxUtilGlobal::StrSearch(ResponseChars, "\r\n\r\n", EndCodeMap, RecvedLength);
					if(0 <= EndCodePos)
					{
						EndCodePos += 4; // "\r\n\r\n"
                        // Content-Length 체크
                        int ContentLengthPos = BxUtilGlobal::StrSearch(ResponseChars, "Content-Length:", ContentLengthMap, EndCodePos);
						if(0 <= ContentLengthPos)
						{
							ContentLengthPos += 15;
							ResponseLength = BxUtilGlobal::AtoI(ResponseChars + ContentLengthPos, EndCodePos - ContentLengthPos);
						}
						// EndCode직후의 숫자도 없을 경우 5MByte로 잡는다
                        else ResponseLength = 1024 * 1024 * 5;
						// Connection 체크
						KeepAlive = (0 <= BxUtilGlobal::StrSearch(ResponseChars, "Connection: keep-alive",
							ConnectionMap, EndCodePos));
						// Set-Cookie 체크
						int CurCookiePos = 0, NextCookiePos = 0;
						while(0 <= (NextCookiePos = BxUtilGlobal::StrSearch(ResponseChars + CurCookiePos, "Set-Cookie:",
							SetCookieMap, EndCodePos - CurCookiePos)))
						{
							CurCookiePos += NextCookiePos + 11;
							BxString::Parse NewName, NewValue;
							while(ResponseChars[CurCookiePos] == ' ') CurCookiePos++;
							while(ResponseChars[CurCookiePos] != '=') NewName += ResponseChars[CurCookiePos++];
							while(ResponseChars[CurCookiePos] != '\r') NewValue += ResponseChars[CurCookiePos++];
							ContactPool::Cookie** OldCookie = LastContact->CookieMap.Access((string) NewName);
							if(OldCookie) (*OldCookie)->SetValue(NewValue);
							else LastContact->CookieMap((string) NewName) = LastContact->CookieList[LAST].Init(NewName, NewValue);
						}
						// 데이터용 버퍼확보
						BxFree(ResponseBytes);
						BxDelete(ResponseJSON);
						ResponseBytes = (byte*) BxAlloc(ResponseLength + 1);
						break;
					}
				}
			}
			// 받은 내용이 없을 경우
			else if(Timeout < (ErrorTime += SleepTime))
				return SetLastError("HTTP header download is timed out"); // 대기시간초과
			else BxCore::System::Sleep(BxUtilGlobal::Min(SleepTime++, Timeout >> 6));
		}
		if(RecvResult < 0)
			return SetLastError("Failed receive HTTP header"); // 수신실패

		// 데이터이관 및 추가수신
		RecvedLength -= EndCodePos;
		if(RecvedLength < ResponseLength)
		{
			BxCore::Util::MemCpy(ResponseBytes, &ResponseChars[EndCodePos], RecvedLength);
			ErrorTime = 0, SleepTime = 0;
			while(0 <= (RecvResult = BxCore::Socket::Recv(LastContact->Socket, ResponseBytes + RecvedLength, ResponseLength - RecvedLength)))
			{
				RecvedLength += RecvResult;
				if(Progress && !Progress(RecvedLength, ResponseLength)) // 프로그레스
					return false;
				// 추가수신완료1 : 정상적인 파일
				if(RecvedLength == ResponseLength) break;
				// 추가수신완료2 : Content-Length가 부정확한 HTML파일
				else if(BxUtilGlobal::StrSameCount((string) &ResponseBytes[RecvedLength - 5], "0\r\n\r\n") == 5)
				{
					ResponseLength = RecvedLength;
					break;
				}
				// 받은 내용이 없을 경우
				if(RecvResult == 0)
				{
					if(Timeout < (ErrorTime += SleepTime))
						return SetLastError("HTTP data download is timed out"); // 대기시간초과
					else BxCore::System::Sleep(BxUtilGlobal::Min(SleepTime++, Timeout >> 6));
				}
				else
				{
					ErrorTime = 0;
					SleepTime >>= 1;
				}
			}
		}
		else BxCore::Util::MemCpy(ResponseBytes, &ResponseChars[EndCodePos], ResponseLength);

		// 잔여데이터 소진
		while(0 < RecvResult)
		{
			char Temp[16];
			RecvResult = BxCore::Socket::Recv(LastContact->Socket, Temp, 16);
			if(Progress && !Progress(-1, 0)) return false; // 업데이트
		}
		// 소켓처리
		if(!KeepAlive)
		{
			BxCore::Socket::Release(LastContact->Socket);
			LastContact->Socket = nullptr;
		}
		// 에러처리
		if(RecvResult < 0)
			return SetLastError("Failed receive HTTP data"); // 수신실패
		ResponseBytes[ResponseLength] = '\0';
		return true;
	}

	/*!
	\brief 캐시파일 로드(없으면 생성)
	\param Domain : 접속할 서버(예: "www.naver.com")
	\param Port : 포트번호(HTTP는 보통 80번포트)
	\param Path : 다운받을 파일 또는 실행할 서블릿의 경로(예: "Update/Abc.bmp")
	\param Arguments : 서블릿의 경우 전달할 인자들(예: "Data0=aaa&Data1=bbb&Data2=ccc")
	\param DoLoad : 메모리에 로딩할지의 여부, false면 다운로드여부 확인만 하겠다는 의미
	\param Progress : 프로그레스의 전달
	\return 캐시로드 성공여부
	\see GetLastError
	*/
	public: bool Cache(string Domain, ushort Port, string Path, string Arguments = nullptr, bool DoLoad = true, callback_progress Progress = nullptr)
	{
		global_data const int CacheCountSumMax = BxCore::System::GetConfigNumber("Bx.WebContent.CacheCountSumMax", -1);
		global_data const int CacheSizeSumMax = BxCore::System::GetConfigNumber("Bx.WebContent.CacheSizeSumMax", -1);

		// 캐시로드
		BxString CacheFileName;
		if(Arguments == nullptr) CacheFileName.Format("<>:<A><FS><A>", BxARG(GetCachePath(), Path));
		else CacheFileName.Format("<>:<A><FS><A><FS><A>", BxARG(GetCachePath(), Path, Arguments));
		int FindedPos = -1;
		while((FindedPos = CacheFileName.Find('?')) != -1)
			CacheFileName[FindedPos] = '_';
		if(BxCore::File::IsExist(CacheFileName))
		{
			id_file CacheFile = BxCore::File::Open(CacheFileName, "rb");
			const int FileSize = BxCore::File::GetSize(CacheFile);
			if(!BxCore::File::IsExist(CacheFileName + ".downloading"))
			{
				if(DoLoad)
				{
					ResponseLength = FileSize;
					BxFree(ResponseBytes);
					BxDelete(ResponseJSON);
					ResponseBytes = (byte*) BxAlloc(ResponseLength + 1);
					BxCore::File::Read(CacheFile, ResponseBytes, ResponseLength);
					BxCore::File::Close(CacheFile);
					ResponseBytes[ResponseLength] = '\0';
				}
				else 
				{
					ResponseLength = 0;
					BxFree(ResponseBytes);
					BxDelete(ResponseJSON);
				}
				return true;
			}
			BxCore::File::Close(CacheFile);
		}

		// 쿼리실행
		if(!Query(Domain, Port, Path, Arguments, -1, Progress))
			return false;

		// 캐시풀 초기수집
		if(!IsInitPool() && (CacheCountSumMax != -1 || CacheSizeSumMax != -1))
		{
			IsInitPool() = true;
			BxCore::File::SearchFiles(GetCachePath(), OnFileSearch, nullptr, nullptr);
		}

		// 캐시저장
		BxCore::File::MakeDirectory(CacheFileName.GetFilePath());
		// 다운로드중 생성
		id_file DownloadingFile = BxCore::File::Open(CacheFileName + ".downloading", "wb");
		BxCore::File::Write(DownloadingFile, &ResponseLength, 4);
		BxCore::File::Close(DownloadingFile);
		// 실제파일 생성
		id_file CacheFile = BxCore::File::Open(CacheFileName, "wb");
		BxCore::File::Write(CacheFile, ResponseBytes, ResponseLength);
		BxCore::File::Close(CacheFile);
		// 다운로드중 삭제
		BxCore::File::RemoveFile(CacheFileName + ".downloading");
		if(!DoLoad)
		{
			ResponseLength = 0;
			BxFree(ResponseBytes);
			BxDelete(ResponseJSON);
		}

		// 캐시풀 운영
		if(IsInitPool())
		{
			// 제한처리
			const int NewFileSize = BxCore::File::GetSize(CacheFileName);
			while((CacheCountSumMax != -1 && 0 < Pool().Length() && CacheCountSumMax < Pool().Length() + 1) ||
				(CacheSizeSumMax != -1 && 0 < PoolSumSize() && CacheSizeSumMax < PoolSumSize() + NewFileSize))
			{
				int iBest = 0;
				uhuge BestDate = Pool()[0].Date;
				for(int i = 1; i < Pool().Length(); ++i)
					if(Pool()[i].Date < BestDate)
					{
						iBest = i;
						BestDate = Pool()[i].Date;
					}
				// RemoveFile로 지워지지 않는 파일이라고 해도 캐시관리 특성상 지워졌다고 판단함
				BxCore::File::RemoveFile(Pool()[iBest].Path, true);
				PoolSumSize() -= Pool()[iBest].Size;
				Pool().Delete(iBest);
			}
			// 항목추가
			Pool()[LAST].Path = BxUtilGlobal::StrCpyWithAlloc(CacheFileName);
			Pool()[END].Size = NewFileSize;
			Pool()[END].Date = BxCore::File::GetDate(CacheFileName);
			PoolSumSize() += NewFileSize;
		}
		return true;
	}
};
