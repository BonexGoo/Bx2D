#pragma once
#include <BxArgument.hpp>

//! \brief BxEngine Core-API
namespace BxCore
{
	//! \brief Main파트
	namespace Main
	{
		void EventAttach();
		void EventDetach();
		int GetCurrentUpdateCount();
		int GetCurrentRenderCount();
		void SetCurrentFrameTime(int ms);
		int GetCurrentFrameTime(bool onlyFrame = false);
		int GetRealFrameTime();
		int GetGUIMarginL();
		int GetGUIMarginT();
		int GetGUIMarginR();
		int GetGUIMarginB();
		void PushFrameDelay();
		void PullFrameDelay();
		bool Procedure();
		bool ProcedureWithYield(int sleep);
        void RunApplication();
	}

	//! \brief System파트
	namespace System
	{
		/*!
		\brief 어플종료 요청여부
		\return true-종료가 요청됨, false-종료요청 없음
		\see DoQuit
		*/
		bool IsQuit();

		/*!
		\brief 어플종료 요청
		\see IsQuit
		*/
		void DoQuit();

		/*!
		\brief 프로세스 슬립(밀린 소켓/키이벤트등 처리)
		\param ms : 쉴 시간(1/1000초)
		*/
		void Sleep(int ms);

		/*!
		\brief 현재시간을 밀리초로 구하기(천분의 1초)
		\return 1/1,000초 단위로 계산된 현재시간(UTC기준)
		*/
		uhuge GetTimeMilliSecond();

		/*!
		\brief 현재시간을 나노초로 구하기(10억분의 1초)
		\return 1/1,000,000,000초 단위로 계산된 현재시간
		*/
		uhuge GetTimeNanoSecond();

		/*!
		\brief 현재시간을 유사 마이크로초(나노초>>10)로 구하기
		\return 1/976,562초 단위로 계산된 현재시간
		*/
		uhuge GetTimeSimilarMicroSecond();

		/*!
		\brief 멀티터치 가능여부
		\return true-가능, false-불가능
		*/
		bool IsMultiTouchAvailable();

		/*!
		\brief 터치의 현재 누름여부
		\param includeSpecial : 특수터치 포함여부
		\return true-눌러짐, false-안눌러짐
		*/
		bool IsTouchDown(bool includeSpecial = true);

		/*!
		\brief 특정 키의 현재 누름여부
		\param key : 확인할 키값
		\return true-눌러짐, false-안눌러짐
		*/
		bool IsKeyDown(keykind key);

		/*!
		\brief 환경설정값 존재여부
		\param type : 변수타입
		\param name : 변수명
		\return true-있음, false-없음
		*/
		bool IsExistConfig(keyword type, string name);

		/*!
		\brief Check형 환경설정값 불러오기
		\param name : 변수명
		\param defaultCheck : 디폴트값
		\return 해당 설정값
		*/
		bool GetConfigCheck(string name, bool defaultCheck);

		/*!
		\brief Number형 환경설정값 불러오기
		\param name : 변수명
		\param defaultNumber : 디폴트값
		\return 해당 설정값
		*/
		int GetConfigNumber(string name, int defaultNumber);

		/*!
		\brief String형 환경설정값 불러오기
		\param name : 변수명
		\param defaultString : 디폴트값
		\return 해당 설정값
		*/
		string _tmp_ GetConfigString(string name, string defaultString);

		/*!
		\brief 마멀레이드의 Number형 환경설정값(icf/s3e) 불러오기
		\param name : 변수명
		\return 해당 설정값
		*/
		int GetPlatformConfigNumber(string name);

		/*!
		\brief 마멀레이드의 String형 환경설정값(icf/s3e) 불러오기
		\param name : 변수명
		\return 해당 설정값
		*/
		string _tmp_ GetPlatformConfigString(string name);

		/*!
		\brief 시뮬레이터 윈도우위치설정(데스크탑 빌드전용)
		\param x : 위치값X
		\param y : 위치값Y
		*/
		void SetSimulatorWindowPos(int x, int y);

		/*!
		\brief 시뮬레이터 윈도우위치얻기(데스크탑 빌드전용)
		\return 위치값
		*/
		point GetSimulatorWindowPos();

		/*!
		\brief 시뮬레이터 커서위치설정(데스크탑 빌드전용)
		\param x : 위치값X
		\param y : 위치값Y
		*/
		void SetSimulatorCursorPos(int x, int y);

		/*!
		\brief 시뮬레이터 커서위치얻기(데스크탑 빌드전용)
		\return 위치값
		*/
		point GetSimulatorCursorPos();

		/*!
		\brief 시뮬레이터 최소화하기(데스크탑 빌드전용)
		*/
		void DoSimulatorMinimize();

		/*!
		\brief 스냅샷 찍기
		\param filename : 파일명
		*/
		void DoExportSnapshot(string filename);

		/*!
		\brief 패널디버그기능 활성화여부
		\return 활성화여부
		*/
		bool IsEnablePanelDebug();

		/*!
		\brief 폰트디버그기능 활성화여부
		\return 활성화여부
		*/
		bool IsEnableFontDebug();

		/*!
		\brief 실행중지(Macro함수인 BxASSERT로 사용권장)
		\param name : 에러내용
		\param query : 질의내용
		\return 사용자의 선택(yes-디버깅, no-1회무시, ignore-계속무시)
		*/
		assertresult Break(string name, string query __DEBUG_PRM__);

		/*!
		\brief 출력창에 스트링기록(Macro함수인 BxTRACE로 사용권장)
		\param map : 스트링맵 스크립트(식별문자는 "<>:")
		\param args : map에 쓰일 가변인자
		*/
		void Trace(string map, const BxArgument& args = BxArgument::zero());

		/*!
		\brief CP949 스트링으로부터 UTF8 스트링얻기
		\param cp949 : CP949 스트링
		\return UTF8 스트링
		*/
		string const GetUTF8ByCP949(string const cp949);

		/*!
		\brief UTF8 스트링으로부터 CP949 스트링얻기
		\param utf8 : UTF8 스트링
		\return CP949 스트링
		*/
		string const GetCP949ByUTF8(string const utf8);

		/*!
		\brief CP949 스트링상수로부터 UTF8 스트링상수얻기 (Macro함수인 W1을 사용권장)
		\param cp949 : CP949 스트링상수
		\return UTF8 스트링상수
		*/
		string const GetUTF8LiteralByCP949(string const cp949);

		/*!
		\brief UTF8 스트링상수로부터 CP949 스트링상수얻기 (Macro함수인 MB를 사용권장)
		\param utf8 : UTF8 스트링상수
		\return CP949 스트링상수
		*/
		string const GetCP949LiteralByUTF8(string const utf8);

		/*!
		\brief OS이름 구하기
        \return 자기 플랫폼의 OS이름(예: SYMBIAN, WINDOWS, OSX, WINMOBILE, LINUX, WIPI, IPHONE, ANDROID)
		*/
		string _tmp_ GetOSName();

		/*!
		\brief 아키텍쳐이름 구하기
		\return 컴파일된 아키텍쳐이름(예: ARM4T, ARM4, ARM5T, ARM5TE, ARM5TEJ, ARM6, ARM6K, ARM6T2, ARM6Z, ARM7, X86, PPC, AMD64, MIPS)
		*/
		string _tmp_ GetArchName();

		/*!
		\brief 장치ID 구하기
		\param integerid : 정수타입 장치ID 받기
		\return 스트링타입 장치ID
		*/
		string _tmp_ GetDeviceID(int* integerid = nullptr);

		/*!
		\brief 휴대폰번호 구하기
		\return 자신의 휴대폰번호(예: 01022223333)
		*/
		string _tmp_ GetPhoneNumber();

		/*!
		\brief 어플패키지 구하기
        \return 자신의 어플패키지(예: com.tgr.game)
		*/
		string _tmp_ GetAppPackage();

		/*!
		\brief 어플패키지 파일확장자 구하기
		\return 파일확장자(예: .apk)
		*/
		string _tmp_ GetAppPackageExt();

		/*!
		\brief 어플버전 구하기
		\return 자신의 어플버전(예: 1.0.0)
		*/
		string _tmp_ GetAppVersion();

		/*!
		\brief 어플의 존재여부
		\param packagename : 패키지명
		\return 존재시 true, 없을시 false
		\see GetAppPackage
		*/
		bool IsExistApp(string packagename);

		/*!
		\brief 웹페이지연결
		\param url : 연결할 웹주소(예: market://search?q=pname:com.popcap.pvz, tstore://PRODUCT_VIEW/0000252470/1)
		\param exitme : 현재 프로그램의 종료여부
		*/
		void PopupOSExecute(string url, bool exitme);

		/*!
		\brief 가상키보드로 스트링얻기
		\param result_str256 : 얻을 스트링(256짜리 배열권장)
		\param default_str : 처음 에디터에 보일 스트링
		\param keyboard_name : 가상키보드의 이름
		\param type : 키보드종류(기본, 이메일, 숫자, 비번, 웹주소)
		\return 성공여부(미지원 OS를 고려)
		*/
		bool PopupOSKeyboard(string_rw result_str256, string default_str = "", string keyboard_name = "", oskeyboardtype type = oskeyboardtype_base);

		/*!
		\brief 프레임단위 정기적 콜백등록(참조식)
		\param method : 콜백함수(호출된 콜백함수에서 false리턴시 해제)
		\param data : 데이터원본
		\param datasize : -1은 참조식, 0보다 같거나 크면 복사식(false리턴시 함께 해제)
		*/
		void AddCallback(callback_frame method, int value, void* data, int datasize = -1);

		/*!
		\brief 자식프로세스의 실행
		\param process : 프로세스를 진행시킬 콜백함수
		\return 실행여부(여러개의 자식프로세스가 실행될 순 없으므로 false이면 실행거절)
		*/
		bool BeginChildProcess(callback_process process);

		/*!
		\brief JAVA클래스 존재확인
		\param classpath : 클래스주소(예: com/abc/ABC)
		\return 존재시 true, 없을시 false
		*/
		bool IsExistClassJNI(string classpath);

		/*!
		\brief C→JAVA 정적함수호출
		\param classpath : 클래스주소(예: com/abc/ABC)
		\param methodname : 함수이름
		\param param1_str1024 : 전달할 스트링
		\param param2 : 전달할 상수
		\return JAVA함수리턴값, -90001에서 -90002는 JNI연결실패, -90003은 classpath없음
		*/
		int CallStaticMethodJNI(string classpath, string methodname, string param1_str1024, int param2);

		/*!
		\brief JAVA→C 콜백등록(최대 8개까지 등록)
		\param classpath : 클래스주소(예: com/abc/ABC)
		\param methodname : 함수이름
		\param methodCB : 콜백함수
		\return 0-성공, -90001에서 -90002는 JNI연결실패, -90003은 classpath없음, -90004는 methodname없음, -90005는 콜백등록수량초과
		*/
		int AddCallbackJNI(string classpath, string methodname, callback_jni methodCB);

		/*!
		\brief EDK수신용 콜백등록(어떤 스레드에서 호출당하던지 엔진스레드로 비동기수신)
		\param methodCB : 콜백함수
		\return EDK수신용 내부함수 전달
		*/
		callback_edk SetCallbackEDK(callback_edk methodCB);
	}

	//! \brief Surface파트
	namespace Surface
	{
		/*!
		\brief 화면생성
		*/
		void Init();

		/*!
		\brief 업데이트 및 백버퍼를 화면에 출력
		\param ptr : 화면에 출력할 이미지의 포인터
		\param row : 이미지의 너비(byte)
		\param height : 이미지의 높이(pixel)
		\param pitch : 더미를 포함한 이미지의 너비(byte)
		*/
		void Flush(void* ptr, const int row, const int height, const int pitch);

		/*!
		\brief 중간 업데이트
		*/
		void MiddleFlush();

		/*!
		\brief 화면너비 구하기
		\param isfixedvalue : icf의 SurfaceWidthBx를 적용한 고정사이즈 여부조사(nullptr이면 생략)
		\param usemargin : Margin값 적용여부
		\return 너비길이(pixel)
		*/
		int GetWidth(bool* isfixedvalue = nullptr, bool usemargin = true);

		/*!
		\brief 화면높이 구하기
		\param isfixedvalue : icf의 SurfaceHeightBx를 적용한 고정사이즈 여부조사(nullptr이면 생략)
		\param usemargin : Margin값 적용여부
		\return 높이길이(pixel)
		*/
		int GetHeight(bool* isfixedvalue = nullptr, bool usemargin = true);

		/*!
		\brief 하드웨어적 화면너비 구하기
		\param usemargin : Margin값 적용여부
		\return 너비길이(pixel)
		*/
		int GetWidthHW(bool usemargin = true);

		/*!
		\brief 하드웨어적 화면높이 구하기
		\param usemargin : Margin값 적용여부
		\return 높이길이(pixel)
		*/
		int GetHeightHW(bool usemargin = true);

		/*!
		\brief 마진고려한 Width/Height값의 작동여부
		\param enable : 작동여부
		\return 이전 마진모드
		*/
		const bool SetMarginMode(bool enable);
	}

	//! \brief Sound파트
	namespace Sound
	{
		/*!
		\brief 사운드ID 할당(icf상의 [game]SoundCacheSize내에서 할당)
		\param filename : 파일명(mid, mp3, aac, qcp, wav)
		\param autoload : 자동로딩해제시스템
		\return 사운드ID(nullptr은 실패)
		\see Release
		*/
		id_sound Create(string filename, bool autoload = true);

		/*!
		\brief 사운드ID 반환
		\param sound : 사운드ID
		\see Create
		*/
		void Release(id_sound sound);

		/*!
		\brief 배경음 출력
		\param sound : 사운드ID(파노라마시 첫번째 사운드ID)
		\param interrupt : 출력중인 사운드ID를 즉시 중지시키고 새로 출력
		\param panorama : 파노라마로 추가될 사운드ID들
		\see StopMusic
		*/
		void PlayMusic(id_sound sound, bool interrupt = false, const BxArgument& panorama = BxArgument::zero());

		/*!
		\brief 배경음 중지
		\see PlayMusic
		*/
		void StopMusic();

		/*!
		\brief 배경음 볼륨
		\param percent : 백분율(디폴트값 100)
		*/
		void SetMusicVolume(uint percent);

		/*!
		\brief 효과음 출력
		\param sound : 사운드ID
		\param interrupt : 출력중인 효과음중 동일한 사운드ID가 있을때 즉시 중지시키고 새로 출력
		\see StopEffect
		*/
		void PlayEffect(id_sound sound, bool interrupt = false);

		/*!
		\brief 모든 효과음 중지
		\see PlayEffect
		*/
		void StopEffect();

		/*!
		\brief 효과음 볼륨
		\param percent : 백분율(디폴트값 100)
		*/
		void SetEffectVolume(uint percent);

		/*!
		\brief 전체 배경음/효과음 일시정지
		*/
		void PauseAll();

		/*!
		\brief 전체 배경음/효과음 일시정지 해제
		*/
		void ResumeAll();
	}

	//! \brief Util파트
	namespace Util
	{
		/*!
		\brief 메모리할당
		\param size : 요청할 길이(byte)
		\return 할당된 메모리, 실패시 nullptr
		\see Free
		*/
		void* Alloc(int size);

		/*!
		\brief 메모리반환
		\param item : 반환할 메모리
		\return 간편한 사용을 위한 nullptr값 리턴
		\see Alloc
		*/
		void* Free(const void* buf);

		/*!
		\brief 메모리 초기화
		\param dst : 초기화할 시작 메모리번지
		\param value : 초기화시킬 값
		\param length : 초기화할 길이(byte)
		*/
		void MemSet(void* dst, byte value, int length);

		/*!
		\brief 메모리 복사
		\param dst : 붙여넣을 시작 메모리번지
		\param src : 복사할 원본의 시작 메모리번지
		\param length : 복사할 길이(byte)
		*/
		void MemCpy(void* dst, const void* src, int length);

		/*!
		\brief 메모리 비교
		\param dst : 비교대상 시작 메모리번지
		\param src : 비교주체 시작 메모리번지
		\param length : 비교할 길이(byte)
		\return 동일하면 0, 다르면 0이 아님
		*/
		int MemCmp(const void* dst, const void* src, int length);

		/*!
		\brief 스트링구성
		\param map : 스트링맵 스크립트(식별문자는 "<>:")
		<A> : 가변인수참조
		<X...> : 16진수입력
		<B> : 공백문자
		<T> : 탭문자
		<N> : 뉴라인문자
		<R> : 리턴문자
		<Z> : 널문자
		<FS> : 슬래시
		<RS> : 역슬래시
		<DQ> : 쌍따옴표
		<SQ> : 작은따옴표
		\param args : map에 쓰일 가변인자
		\param length : 구성된 스트링의 길이(byte)
		\return 구성된 스트링
		*/
		string _tmp_ Print(string map, const BxArgument& args = BxArgument::zero(), int* length = nullptr);

		const int* Cos1024Table();
		const int* Tan1024Table();
		wstring UTF16Table();
		wstring CP949Table();
	}

	//! \brief AddOn파트
	namespace AddOn
	{
		/*!
		\brief JPG이미지로 BMP를 제작
		\param jpg : JPG리소스
		\return BMP리소스(Release로 해제)
		\see Release
		*/
		const byte* JPGToBMP(const byte* jpg);

		/*!
		\brief GIF이미지로 BMP를 제작
		\param gif : GIF리소스
		\param length : GIF리소스의 길이(byte)
		\param numpage : 장면수
		\return 모든 장면이 세로로 연결된 BMP리소스(Release로 해제)
		\see Release
		*/
		const byte* GIFToBMP(const byte* gif, const int length, int _out_ numpage);

		/*!
		\brief HQX방식으로 재가공된 BMP를 제작
		\param bmp : BMP리소스
		\param scale : HQX옵션(2, 3, 4중 선택)
		\return 재가공된 BMP리소스(Release로 해제)
		\see Release
		*/
		const byte* HQXToBMP(const byte* bmp, int scale);

		/*!
		\brief 데이터버퍼를 통해 MD5해시스트링을 제작
		\param buffer : 데이터버퍼
		\param length : 데이터버퍼의 길이(byte)
		\return MD5해시스트링(무조건 32바이트)
		*/
		string _tmp_ BUFToMD5(const byte* buffer, const int length);

		/*!
		\brief 데이터버퍼를 통해 BASE64버퍼를 제작
		\param buffer : 데이터버퍼
		\param length : 데이터버퍼의 길이(byte)
		\param get_base64_length : nullptr이 아닌 경우 BASE64버퍼의 길이(byte)를 받음
		\return BASE64버퍼
		\see Release
		*/
		const byte* BUFToBASE64(const byte* buffer, const int length, int* get_base64_length = nullptr);

		/*!
		\brief BASE64버퍼를 통해 데이터버퍼를 제작
		\param base64 : BASE64버퍼
		\param length : BASE64버퍼의 길이(byte)
		\param get_buffer_length : nullptr이 아닌 경우 데이터버퍼의 길이(byte)를 받음
		\return 데이터버퍼
		\see Release
		*/
		const byte* BASE64ToBUF(const byte* base64, const int length, int* get_buffer_length = nullptr);

		/*!
		\brief ZIP파일의 디코딩을 위한 객체 생성
		\param zip : ZIP리소스
		\param length : ZIP리소스의 길이(byte)
		\param numfile : 파일수
		\param password : 비밀번호
		\return ZIP객체
		\see ReleaseZIP
		*/
		id_zip CreateZIP(const byte* zip, const int length, int _out_ numfile, string password = nullptr);

		/*!
		\brief ZIP객체 제거
		\param zip : ZIP객체
		\see CreateZIP
		*/
		void ReleaseZIP(id_zip zip);

		/*!
		\brief ZIP객체에서 요청 순번의 파일을 로드
		\param zip : ZIP객체
		\param fileindex : 요청 순번
		\param filesize : 파일사이즈
		\return 요청 순번의 파일리소스(Release로 해제)
		\see Release
		*/
		const byte* ZIPToFILE(id_zip zip, const int fileindex, int _out_ filesize);

		/*!
		\brief ZIP객체에서 요청 순번의 파일정보를 로드
		\param zip : ZIP객체
		\param fileindex : 요청 순번
		\param isdir : 폴더인지의 여부
		\param ctime : 생성시간(UTC기준)
		\param mtime : 수정시간(UTC기준)
		\param atime : 접근시간(UTC기준)
		\param archive : 저장속성 여부
		\param hidden : 숨김속성 여부
		\param readonly : 읽기전용속성 여부
		\param system : 시스템속성 여부
		\return 요청 순번의 파일명
		*/
		string _tmp_ ZIPToINFO(id_zip zip, const int fileindex,
			bool* isdir = nullptr, uhuge* ctime = nullptr, uhuge* mtime = nullptr, uhuge* atime = nullptr,
			bool* archive = nullptr, bool* hidden = nullptr, bool* readonly = nullptr, bool* system = nullptr);

		/*!
		\brief TTF파일의 랜더링을 위한 객체 생성
		\param ttf : TTF리소스
		\param length : TTF리소스의 길이(byte)
		\return TTF객체
		\see ReleaseTTF
		*/
		id_ttf CreateTTF(const byte* ttf, const int length);

		/*!
		\brief TTF객체 제거
		\param ttf : TTF객체
		\see CreateTTF
		*/
		void ReleaseTTF(id_ttf ttf);

		/*!
		\brief TTF객체에서 해당 문자를 BMP로 제작
		\param ttf : TTF객체
		\param height : 세로길이(pixel)
		\param code : 문자코드(UTF16)
		\return BMP리소스(Release로 해제)
		\see Release
		*/
		const byte* TTFToBMP(id_ttf ttf, int height, uint code);

		/*!
		\brief TTF객체에서 해당 문자정보를 로드
		\param ttf : TTF객체
		\param height : 세로길이(pixel)
		\param code : 문자코드(UTF16)
		\param width : 가로길이(pixel)
		\param ascent : 기준라인위치(pixel)
		\return 성공여부
		*/
		bool TTFToINFO(id_ttf ttf, int height, uint code, int* width = nullptr, int* ascent = nullptr);

		/*!
		\brief 크기정보로 BMP를 생성(픽셀정보는 초기화되지 않음)
		\param width : 가로길이(pixel)
		\param height : 세로길이(pixel)
		\param param1 : 추가정보1
		\param param2 : 추가정보2
		\return BMP리소스(Release로 해제)
		\see Release
		*/
		const byte* CreateBMP(int width, int height, short param1 = 0, short param2 = 0);

		/*!
		\brief BMP의 가로길이(pixel)
		\param bmp : BMP리소스
		\return 가로길이(pixel)
		\see CreateBMP
		*/
		int GetBMPWidth(const byte* bmp);

		/*!
		\brief BMP의 세로길이(pixel)
		\param bmp : BMP리소스
		\return 세로길이(pixel)
		\see CreateBMP
		*/
		int GetBMPHeight(const byte* bmp);

		/*!
		\brief BMP의 추가정보1(short)
		\param bmp : BMP리소스
		\return 추가정보1(short)
		\see CreateBMP
		*/
		short GetBMPParam1(const byte* bmp);

		/*!
		\brief BMP의 추가정보2(short)
		\param bmp : BMP리소스
		\return 추가정보2(short)
		\see CreateBMP
		*/
		short GetBMPParam2(const byte* bmp);

		/*!
		\brief BMP의 픽셀정보(bitmappixel)
		\param bmp : BMP리소스
		\return 픽셀정보(bitmappixel) 시작포인터
		\see CreateBMP
		*/
		bitmappixel* GetBMPBits(const byte* bmp);

		/*!
		\brief AddOn리소스 해제
		\param buf : AddOn함수로 생성한 메모리
		*/
		void Release(const byte* buf);
	}

	//! \brief File파트
	namespace File
	{
		/*!
		\brief 파일의 존재여부
		\param filename : 파일명
		\return 존재시 true, 없을시 false
		\see Open
		*/
		bool IsExist(string filename);

		/*!
		\brief 파일열기
		\param filename : 파일명
		\param mode : 열기옵션(전:"r"/"w"/"a"/"r+"/"w+"/"a+", 후:"t"/"b"/"U")
		\return 열려진 파일ID, 실패시 0
		\see IsExist, Close, Attach
		*/
		id_file Open(string filename, string mode);

		/*!
		\brief 파일닫기
		\param file : 열린 파일ID
		\see Open
		*/
		void Close(id_file file);

		/*!
		\brief 파일에서 읽기
		\param file : 파일ID
		\param buffer : 읽은 내용을 저장할 메모리
		\param length : 읽기요청할 길이(byte)
		\param progress : 프로그레스의 전달(업데이트전용, progress(-1, 0)로 호출)
		\return 읽은 길이(byte)
		*/
		uint Read(id_file file, void* buffer, uint length, callback_progress progress = nullptr);

		/*!
		\brief 파일에서 가변정수읽기
		\param file : 파일ID
		\param variable : 가변정수를 읽을 메모리
		\param additionkey : 입력보정을 위한 덧셈키
		\param progress : 프로그레스의 전달(업데이트전용, progress(-1, 0)로 호출)
		\return 읽은 길이(byte)
		*/
		uint ReadVariable(id_file file, uhuge* variable, const byte additionkey = 0x00, callback_progress progress = nullptr);

		/*!
		\brief 파일에서 한줄의 스트링읽기
		\param file : 파일ID
		\param tab : 탭용 문자열
		\return 읽은 스트링(최대 4096자, 못 읽었을 경우 nullptr리턴)
		*/
		string _tmp_ ReadLine(id_file file, string tab = "\t");

		/*!
		\brief 파일로 쓰기
		\param file : 파일ID
		\param buffer : 쓸 내용을 담고 있는 메모리
		\param length : 쓰기요청할 길이(byte)
		\param progress : 프로그레스의 전달(업데이트전용, progress(-1, 0)로 호출)
		\return 쓴 길이(byte)
		*/
		uint Write(id_file file, const void* buffer, uint length, callback_progress progress = nullptr);

		/*!
		\brief 파일로 가변정수쓰기
		\param file : 파일ID
		\param variable : 파일에 쓸 가변정수
		\param additionkey : 출력보정을 위한 덧셈키
		\param progress : 프로그레스의 전달(업데이트전용, progress(-1, 0)로 호출)
		\return 쓴 길이(byte)
		*/
		uint WriteVariable(id_file file, const uhuge variable, const byte additionkey = 0x00, callback_progress progress = nullptr);

		/*!
		\brief 파일로 스트링쓰기
		\param file : 파일ID
		\param buffer : 쓸 내용을 담고 있는 스트링
		\param progress : 프로그레스의 전달(업데이트전용, progress(-1, 0)로 호출)
		\return 쓴 길이(byte)
		*/
		uint WriteString(id_file file, string buffer, callback_progress progress = nullptr);

		/*!
		\brief 파일위치 스킵하기
		\param file : 파일ID
		\param length : 스킵요청할 길이(byte)
		\return 결과 파일위치
		*/
		int Skip(id_file file, uint length);

		/*!
		\brief 파일의 전체길이(byte) 얻기
		\param file : 파일ID
		\return 전체길이(byte)
		*/
		int GetSize(id_file file);

		/*!
		\brief 파일의 전체길이(byte) 얻기
		\param filename : 파일명
		\return 전체 길이(byte, 파일이 없으면 -1)
		*/
		int GetSize(string filename);

		/*!
		\brief 파일의 마지막 수정날짜 얻기
		\param filename : 파일명
		\return 마지막 수정날짜(1970년1월1일기준 밀리초환산값)
		*/
		uhuge GetDate(string filename);

		/*!
		\brief DZ압축파일의 연동(여러번 Attach하면 마지막 Attach가 최우선순위)
		\param archivename : 압축파일명
		\return 연동의 성공여부
		\see Detach
		*/
		bool Attach(string archivename);

		/*!
		\brief DZ압축파일의 연동해제(Attach에 성공한 횟수만큼 해제)
		\see Attach
		*/
		void Detach();

		/*!
		\brief 폴더만들기
		\param dirname : 폴더명
		\param isFile : dirname이 파일인지의 여부
		*/
		void MakeDirectory(string dirname, bool isFile = false);

		/*!
		\brief 파일검색
		\param dirname : 폴더명
		\param resultCB : 결과받을 콜백함수
		\param data1 : 콜백함수에 전달할 데이터1
		\param data2 : 콜백함수에 전달할 데이터2
		\return 검색된 수량
		*/
		uint SearchFiles(string dirname, callback_filesearch resultCB, void* data1, void* data2);

		/*!
		\brief 파일삭제
		\param filename : 파일명
		\param doRemoveBlankedDirectory : 빈 폴더가 생기는 경우 폴더까지 삭제할지 여부
		\return 성공여부
		*/
		bool RemoveFile(string filename, bool doRemoveBlankedDirectory = false);

		/*!
		\brief 파일의 이름변경
		\param srcname : 대상 파일명
		\param dstname : 변경하고자 하는 파일명
		\return 성공여부
		*/
		bool RenameFile(string srcname, string dstname);

        /*!
        \brief 카메라폴더로 복사
        \param filename : 파일명
        \return 성공여부
        */
        bool CopyToCamera(string filename);
	}

	//! \brief Socket파트
	namespace Socket
	{
		/*!
		\brief 소켓ID 할당
		\return 소켓ID(nullptr은 실패)
		\see Release, Connect
		*/
		id_socket Create();

		/*!
		\brief 소켓ID 반환, 접속종료
		\param sock : 소켓ID
		\see Create
		*/
		void Release(id_socket sock);

		/*!
		\brief 소켓 상태
		\param sock : 소켓ID
		\return 해당 소켓의 상태
		*/
		socketstate GetState(id_socket sock);

		/*!
		\brief 서버로 접속
		\param sock : 소켓ID
		\param addr : 도메인 또는 IP
		\param port : 포트번호
		\param timeout : 결과를 기다릴 시간
		\param progress : 프로그레스의 전달(업데이트전용, progress(-1, 0)로 호출)
		\return 결과를 기다릴 경우 dowait_connected/dowait_disconnected, 안 기다릴 경우 connecting
		\see Disconnect, GetState
		*/
		connectresult Connect(id_socket sock, string addr, ushort port, uint timeout = 0, callback_progress progress = nullptr);

		/*!
		\brief 서버와의 접속종료
		\param sock : 소켓ID
		\see Connect, GetState
		*/
		void Disconnect(id_socket sock);

		/*!
		\brief 데이터 송신
		\param sock : 소켓ID
		\param buffer : 송신할 버퍼
		\param len : 송신할 길이(byte)
		\return 송신된 길이(byte, 음수는 수신에러코드, 이때 GetState로 Create/Connect를 판단)
		\see GetState
		*/
		int Send(id_socket sock, const void* buffer, int len);

		/*!
		\brief 데이터 수신
		\param sock : 소켓ID
		\param buffer : 수신할 버퍼
		\param len : 버퍼의 길이(byte)
		\return 수신된 길이(byte, 음수는 수신에러코드, 이때 GetState로 Create/Connect를 판단)
		\see GetState
		*/
		int Recv(id_socket sock, void* buffer, int len);

		/*!
		\brief 데이터 강제수신
		\param sock : 소켓ID
		\param buffer : 수신할 버퍼
		\param len : 수신할 길이(byte)
		\param timeout : 결과를 기다릴 시간(-1은 디폴트값)
		\param cancelmode : 탈출가능여부(현재 수신된 데이터가 0일 경우만 작동)
		\return 수신된 길이(byte, 음수는 수신에러코드, 이때 GetState로 Create/Connect를 판단)
		\see GetState
		*/
		int RecvFully(id_socket sock, byte* buffer, int len, int timeout, bool cancelmode);

		/*!
		\brief 핑시간 측정(ICMP프로토콜)
		\param addr : 도메인 또는 IP
		\param timeout : 결과를 기다릴 시간
		\return 핑시간(밀리초, 음수면 실패)
		*/
		int Ping(string addr, uint timeout);
	}

	//! \brief Bluetooth파트
	namespace Bluetooth
	{
		//! \brief Scan파트
		namespace Scan
		{
			void Begin();
			void End();
			int CountOfQueue();
			string GetUuidFromQueue();
			string UuidToName(string uuid);
		}

		id_bluetooth Connect(string uuid);
		void Disconnect(id_bluetooth bluetooth);
		bool IsConnected(id_bluetooth bluetooth);
		bool Send(id_bluetooth bluetooth, const void* buffer, int len);
		int CountOfRecv();
		const void* Recv(id_bluetooth bluetooth, int _out_ len);
		int CountOfEvent();
		string GetEvent(id_bluetooth bluetooth);
	}

	//! \brief Font파트
	namespace Font
	{
		/*!
		\brief 폰트열기
		\param nick : 폰트별칭(미생성 폰트별칭도 사용가능)
		\param height : 세로길이(pixel)
		\return 폰트ID
		\see Close, DefineByFile, DefineByBuffer
		*/
		id_font Open(string nick, int height);

		/*!
		\brief 폰트닫기
		\param font : 폰트ID
		\return 간편한 nullptr리턴
		\see Open
		*/
		id_font_zero Close(id_font font);

		/*!
		\brief 간편한 기본폰트
		\return 기본폰트
		*/
		id_font Default();

		/*!
		\brief 폰트별칭의 생성
		\param nick : 생성할 폰트별칭
		\param filename : TTF파일의 경로
		\return 성공여부
		\see NickClose, IsExistNick
		*/
		bool NickOpen(string nick, string filename);

		/*!
		\brief 폰트별칭의 생성(버퍼)
		\param nick : 생성할 폰트별칭
		\param buffer : TTF버퍼
		\param buffersize : TTF버퍼사이즈
		\return 성공여부
		\see NickClose, IsExistNick
		*/
		bool NickOpenByBuffer(string nick, const byte* buffer, int buffersize);

		/*!
		\brief 폰트별칭의 해제
		\param nick : 해제할 폰트별칭
		\see NickOpen, NickOpenByBuffer
		*/
		void NickClose(string nick);

		/*!
		\brief 폰트별칭의 존재여부
		\param nick : 확인할 폰트별칭
		\return 존재여부
		\see NickOpen, NickOpenByBuffer
		*/
		bool IsExistNick(string nick);

		/*!
		\brief 텍스트의 생성
		\param str : 스트링
		\param encoding : 스트링의 인코딩
		\return 텍스트ID
		\see TextClose
		*/
		id_text TextOpen(string str, textencoding encoding = textencoding_cp949);

		/*!
		\brief 텍스트의 생성(UTF16)
		\param wstr : 스트링(UTF16)
		\param referonly : 참조방식여부(복사과정이 생략)
		\return 텍스트ID
		\see TextClose
		*/
		id_text TextOpenByUTF16(wstring wstr, bool referonly = false);

		/*!
		\brief 텍스트의 해제
		\param text : 텍스트ID
		\return 간편한 nullptr리턴
		\see TextOpen, TextOpenByUTF16
		*/
		id_text_zero TextClose(id_text text);

		/*!
		\brief 폰트의 옵션
		\param font : 폰트ID
		\param color : 색상
		\param opacity : 불투명도
		\param align : 정렬방식
		\see Open
		*/
		void SetOption(id_font font, color_x888 color, byte opacity = 0xFF, textalign align = textalign_left_top);

		/*!
		\brief 폰트의 회전상태(문자단위 중점기준)
		\param font : 폰트ID
		\param angle1024 : 1024도기준 회전값
		\see Open
		*/
		void SetRotate(id_font font, int angle1024 = 0);

		/*!
		\brief 폰트의 양쪽정렬
		\param font : 폰트ID
		\param column : 컬럼의 길이(pixel, 0은 양쪽정렬 해제)
		\see Open
		*/
		void SetJustify(id_font font, int column = 0);

		/*!
		\brief 텍스트기준 폰트의 출력될 너비길이(pixel)
		\param font : 폰트ID
		\param text : 텍스트ID
		\param offset : 시작위치(letter)
		\param length : 사용구간(letter, -1은 자동측정)
		\return 너비길이(pixel)
		\see Open, TextOpen, TextOpenByUTF16
		*/
		int GetWidth(id_font font, id_text text, int offset = 0, int length = -1);

		/*!
		\brief 폰트의 출력될 높이길이(pixel)
		\param font : 폰트ID
		\return 높이길이(pixel)
		\see Open
		*/
		int GetHeight(id_font font);

		/*!
		\brief 단어기준 컬럼내 텍스트의 유효길이(letter)
		\param font : 폰트ID
		\param column : 컬럼의 길이(pixel)
		\param text : 텍스트ID
		\param offset : 시작위치(letter)
		\param width : [out] 유효한 너비길이(pixel)
		\return 유효길이(letter)
		\see Open, TextOpen, TextOpenByUTF16
		*/
		int GetLength(id_font font, int column, id_text text, int offset = 0, int* width = nullptr);

		/*!
		\brief 문자기준 컬럼내 텍스트의 유효길이(letter)
		\param font : 폰트ID
		\param column : 컬럼의 길이(pixel)
		\param text : 텍스트ID
		\param offset : 시작위치(letter)
		\param width : [out] 유효한 너비길이(pixel)
		\return 유효길이(letter)
		\see Open, TextOpen, TextOpenByUTF16
		*/
		int GetLengthByLetter(id_font font, int column, id_text text, int offset = 0, int* width = nullptr);

		/*!
		\brief 텍스트 출력요청(RequestFlush로 일괄출력)
		\param font : 폰트ID
		\param pos : 출력위치(pixel)
		\param text : 텍스트ID
		\param offset : 시작위치(letter)
		\param length : 사용구간(letter, -1은 자동측정)
		\return 출력된 텍스트의 우측하단위치(pixel)
		\see RequestFlush, Open, TextOpen, TextOpenByUTF16
		*/
		point DrawRequest(id_font font, point pos, id_text text, int offset = 0, int length = -1);

		/*!
		\brief 폰트기준 출력요청된 텍스트들을 텍스쳐소팅하여 일괄출력
		\param font : 폰트ID
		\see DrawRequest, Open
		*/
		void RequestFlush(id_font font);
	}

	//! \brief Thread파트
	namespace Thread
	{
		/*!
		\brief 스레드 생성
		\param threadCB : 콜백함수
		\param data : 전달할 데이터
		\return 생성된 스레드ID
		\see Release
		*/
		id_thread Create(callback_thread threadCB, void* data = nullptr);

		/*!
		\brief 스레드 해제
		\param thread : 스레드ID
		\param dokill : 강제로 종료시킬지의 여부
		\param dowait : 종료때까지 대기할지의 여부
		\see Create
		*/
		void Release(id_thread thread, bool dokill = false, bool dowait = false);

		/*!
		\brief 뮤텍스 열기
		\return 생성된 뮤텍스ID
		\see CloseMutex
		*/
		id_mutex OpenMutex();
		
		/*!
		\brief 뮤텍스 닫기
		\param mutex : 뮤텍스ID
		\see OpenMutex
		*/
		void CloseMutex(id_mutex mutex);
		
		/*!
		\brief 뮤텍스 잠금
		\param mutex : 뮤텍스ID
		\see Unlock
		*/
		void Lock(id_mutex mutex);
		
		/*!
		\brief 뮤텍스 해제
		\param mutex : 뮤텍스ID
		\see Lock
		*/
		void Unlock(id_mutex mutex);

		/*!
		\brief 현 스레드상의 저장소 개별연결(스레드수량 x 정적변수수량)
		\param storagekey : thread_storage를 통해서 선언된 연결변수
		\return 저장소의 버퍼주소(처음 생성시 모든 값은 0으로 초기화됨)
		\see UnbindStorageAll
		*/
		void* BindStorage(int* storagekey);

		/*!
		\brief 현 스레드상의 저장소 전체종료
		\see BindStorage
		*/
		void UnbindStorageAll();
	}

	//! \brief Library파트
	namespace Library
	{
		/*!
		\brief 라이브러리 열기
		\param filename : 파일명
		\return 라이브러리ID(nullptr은 실패)
		\see Close
		*/
		id_library Open(string filename);

		/*!
		\brief 라이브러리 닫기
		\param handle : 라이브러리ID
		\see Open
		*/
		void Close(id_library handle);

		/*!
		\brief 함수연결
		\param handle : 라이브러리ID
		\param name : 함수명
		\see 함수포인터
		*/
		void* Link(id_library handle, string name);
	}

	//! \brief OpenGL2D파트
	namespace OpenGL2D
	{
		void Init();
		void Quit();
		void Flush();
		// View
		void SetPerspective(const int mul, const int far, const int near);
		void SetOrthogonal(const int width, const int height, const int far, const int near);
		void AddViewTrans(int x, int y, int z);
		void AddViewRotX(fint angle);
		void AddViewRotY(fint angle);
		void AddViewRotZ(fint angle);
		// Make/Free
		id_opengl_form MakeForm_TextureMaterial(string filename);
		id_opengl_form MakeForm_TextureMaterial(color_a888* image, const int width, const int height, const int pitch);
		id_opengl_form MakeForm_ColorAmbient(const color_x888 color);
		void FreeForm(id_opengl_form form);
		id_opengl_outline MakeOutline_VertexUV(const rect& r, const rect& uvclip);
		id_opengl_outline MakeOutline_PolyVertexUV(const points& p, const rect& uvclip);
		id_opengl_outline MakeOutline_PolyVertexUV3D(const vertexs& v, const points& uv);
		id_opengl_outline MakeOutline_SplineVertex(const spline& s);
		void FreeOutline(id_opengl_outline outline);
		// Get
		color_x888 Form_ColorAmbient_GetColor(id_opengl_form form);
		void Outline_VertexUV_SetScale(id_opengl_outline outline, fint hoz, fint ver);
		void Outline_VertexUV_SetRotate(id_opengl_outline outline, int angle1024, bool doFlip);
		void Outline_PolyVertexUV_SetScale(id_opengl_outline outline, fint hoz, fint ver);
		void Outline_PolyVertexUV_SetRotate(id_opengl_outline outline, int angle1024, bool doFlip);
		// Render
		void Render(id_opengl_form form, id_opengl_outline outline, int x, int y, const byte opacity, const color_x888 color);
		void Render3D(id_opengl_form form, id_opengl_outline outline, int x, int y, int z, const byte opacity, const color_x888 color);
		// Option
		void Clip(rect r);
		// Property
		const int GetTextureMargin();
		const bool DoTextureInterpolation();
	}
}
