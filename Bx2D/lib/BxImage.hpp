#pragma once
#include <BxDraw.hpp>

//! \brief 이미지 운영
class BxImage : public BxDrawForm
{
public:
	enum type {BMP, B16, B32, PNG, JPG, GIF};
	enum hotspot {KEEP = -1, PAD1 = 0, PAD2, PAD3, PAD4, PAD5, PAD6, PAD7, PAD8, PAD9};

	global_func int& ScaleNumber() {global_data int Scale = 1; return Scale;}

protected:
	/// @cond SECTION_NAME
	pixel_src* Bits;
	/// @endcond

public:
	/*!
	\brief 생성자
	*/
	BxImage() : Bits(nullptr) {}
	BxImage(const byte* Resource, const int Length, type Type, hotspot Hotspot = KEEP) : Bits(nullptr) {Load(Resource, Length, Type, Hotspot);}
	BxImage(string FileName, type Type, hotspot Hotspot = KEEP) : Bits(nullptr) {Load(FileName, Type, Hotspot);}
	BxImage(string FileName, type Type, const int X, const int Y, const int Width, const int Height, hotspot Hotspot = PAD7) : Bits(nullptr)
	{Load(FileName, Type, X, Y, Width, Height, Hotspot);}

	/*!
	\brief 소멸자
	*/
	virtual ~BxImage() {Release();}

	/*!
	\brief 이미지 로드
	\param Resource : 메모리리소스
	\param Length : 메모리리소스의 길이(byte, GIF만 필요)
	\param Type : 리소스타입(BMP, B16, B32, PNG, JPG, GIF)
	\param Hotspot : 초기중점(NumPad방식, KEEP은 생략)
	\return 로딩성공여부
	*/
	bool Load(const byte* Resource, const int Length, type Type, hotspot Hotspot = KEEP)
	{
		switch(Type)
		{
		case BMP:
			{
				const byte* ScaledBmp = BxCore::AddOn::HQXToBMP(Resource, ScaleNumber());
				bool Result = _LoadBMP((ScaledBmp)? ScaledBmp : Resource);
				BxCore::AddOn::Release(ScaledBmp);
				SetHotspot(Hotspot);
				return Result;
			}
			break;
		case B16: return _LoadB16(Resource);
		case B32: return _LoadB32(Resource);
		case PNG: return _LoadPNG(Resource);
		case JPG: return _LoadJPG(Resource);
		case GIF: return _LoadGIF(Resource, Length);
		}
		SetHotspot(Hotspot);
		return false;
	}

	/*!
	\brief 이미지 로드
	\param FileName : 파일명
	\param Type : 리소스타입(BMP, B16, B32, PNG, JPG, GIF)
	\param Hotspot : 초기중점(NumPad방식, KEEP은 생략)
	\return 로딩성공여부
	*/
	bool Load(string FileName, type Type, hotspot Hotspot = KEEP)
	{
		id_file File = BxCore::File::Open(FileName, "rb");
		int Length = BxCore::File::GetSize(File);
		byte* Resource = nullptr;
		if(Type == BMP)
		{
			Length += 2;
			Resource = BxNew_Array(byte, Length);
			Resource[0] = '_';
			Resource[1] = '_';
			BxCore::File::Read(File, &Resource[2], Length - 2);
			BxCore::File::Close(File);
		}
		else
		{
			Resource = BxNew_Array(byte, Length);
			BxCore::File::Read(File, Resource, Length);
			BxCore::File::Close(File);
		}
		bool Result = Load(Resource, Length, Type);
		BxDelete_Array(Resource);
		SetHotspot(Hotspot);
		return Result;
	}

	/*!
	\brief 이미지 부분로드
	\param FileName : 파일명
	\param Type : 리소스타입(BMP, B16, B32, PNG, JPG, GIF)
	\param X : 잘라낼 위치X
	\param Y : 잘라낼 위치Y
	\param Width : 잘라낼 너비
	\param Height : 잘라낼 높이
	\param Hotspot : 초기중점(NumPad방식, KEEP은 생략)
	\return 로딩성공여부
	*/
	bool Load(string FileName, type Type, const int X, const int Y, const int Width, const int Height, hotspot Hotspot = PAD7)
	{
		BxImage Src;
		if(!Src.Load(FileName, Type))
			return false;
		Create(Src, X, Y, Width, Height);
		SetHotspot(Hotspot);
		return true;
	}

	/*!
	\brief 이미지 존재여부
	\return 존재여부
	\see Load, Create
	*/
	bool IsExist()
	{
		return _IsLoaded();
	}

	/*!
	\brief 중점설정
	\param HotspotX : 중점X
	\param HotspotY : 중점Y
	*/
	void SetHotspot(const int HotspotX, const int HotspotY)
	{
		Area.hx = HotspotX;
		Area.hy = HotspotY;
	}

	/*!
	\brief 중점설정(NumPad)
	\param Hotspot : NumPad식 중점
	*/
	void SetHotspot(hotspot Hotspot)
	{
		switch(Hotspot)
		{
		case PAD1: Area.hx = 0;          Area.hy = Area.h; break;
		case PAD2: Area.hx = Area.w / 2; Area.hy = Area.h; break;
		case PAD3: Area.hx = Area.w;     Area.hy = Area.h; break;
		case PAD4: Area.hx = 0;          Area.hy = Area.h / 2; break;
		case PAD5: Area.hx = Area.w / 2; Area.hy = Area.h / 2; break;
		case PAD6: Area.hx = Area.w;     Area.hy = Area.h / 2; break;
		case PAD7: Area.hx = 0;          Area.hy = 0; break;
		case PAD8: Area.hx = Area.w / 2; Area.hy = 0; break;
		case PAD9: Area.hx = Area.w;     Area.hy = 0; break;
		}
	}

	/*!
	\brief 빈 이미지 생성
	\param Width : 생성낼 너비
	\param Height : 생성낼 높이
	\param HotspotX : 중점X
	\param HotspotY : 중점Y
	*/
	void Create(const int Width, const int Height, const int HotspotX, const int HotspotY)
	{
		Area.w = Width;
		Area.h = Height;
		ExpressAreaW.Reset(BxInteger(0) + (int) Area.w);
		ExpressAreaH.Reset(BxInteger(0) + (int) Area.h);
		Area.hx = HotspotX;
		Area.hy = HotspotY;
		Flag = blendflag_null;
		Page = 1;
		FrameBegin = 0;
		const int AreaWidth = BxDrawForm::CalcTextureSize(Width);
		const int AreaHeight = BxDrawForm::CalcTextureSize(Height);
		#ifdef __BX_OPENGL
		if(FormGL)
		{
			BxCore::OpenGL2D::FreeForm(FormGL);
			FormGL = nullptr;
		}
		#endif
		BxDelete_Array(Bits);
		Bits = BxNew_Array(pixel_src, AreaWidth * AreaHeight);
		BxCore::Util::MemSet(Bits, 0, sizeof(pixel_src) * AreaWidth * AreaHeight);
	}

	/*!
	\brief 복사식 이미지 생성
	\param Src : 소스이미지
	\param X : 복사할 위치X
	\param Y : 복사할 위치Y
	\param Width : 복사할 너비
	\param Height : 복사할 높이
	\param Hotspot : 초기중점(NumPad방식, KEEP은 생략)
	*/
	void Create(BxImage& Src, const int X, const int Y, const int Width, const int Height, hotspot Hotspot = KEEP)
	{
		Create(Width, Height, Src.Area.hx - X, Src.Area.hy - Y);
		CopyFrom(0, 0, Src, X, Y, Width, Height);
		SetHotspot(Hotspot);
	}

	/*!
	\brief 이미지 제거
	\see Load, Create
	*/
	void Release()
	{
		#ifdef __BX_OPENGL
		if(FormGL)
		{
			BxCore::OpenGL2D::FreeForm(FormGL);
			FormGL = nullptr;
		}
		#endif
		BxDelete_Array(Bits);
	}

	/*!
	\brief 이미지복사
	\param X : 복사될 위치X
	\param Y : 복사될 위치Y
	\param Src : 소스이미지
	\param SrcX : 복사할 소스위치X
	\param SrcY : 복사할 소스위치Y
	\param SrcWidth : 복사할 소스너비
	\param SrcHeight : 복사할 소스높이
	*/
	void CopyFrom(int X, int Y, BxImage& Src, int SrcX, int SrcY, const int SrcWidth, const int SrcHeight)
	{
		if(Area.w <= 0 || Area.h <= 0 || Src.Area.w <= 0 || Src.Area.h <= 0) return;
		int SrcX2 = BxUtilGlobal::Min(SrcX + SrcWidth, Src.Area.w);
		int SrcY2 = BxUtilGlobal::Min(SrcY + SrcHeight, Src.Area.h);
		if(X < 0) SrcX -= X, X = 0;
		if(Y < 0) SrcY -= Y, Y = 0;
		if(SrcX < 0) X -= SrcX, SrcX = 0;
		if(SrcY < 0) Y -= SrcY, SrcY = 0;
		SrcX2 = BxUtilGlobal::Min(SrcX2 - SrcX, Area.w - X) + SrcX;
		SrcY2 = BxUtilGlobal::Min(SrcY2 - SrcY, Area.h - Y) + SrcY;
		if(SrcX2 - SrcX <= 0 || SrcY2 - SrcY <= 0) return;

		Flag = (blendflag)(Flag | Src.Flag);
		const int AreaWidth = BxDrawForm::CalcTextureSize(Area.w);
		const int AreaWidthSrc = BxDrawForm::CalcTextureSize(Src.Area.w);
		const int CopyLength = sizeof(pixel_src) * (SrcX2 - SrcX);
		for(int y = Y, srcy = SrcY; srcy < SrcY2; ++y, ++srcy)
			BxCore::Util::MemCpy(&Bits[X + y * AreaWidth], &Src.Bits[SrcX + srcy * AreaWidthSrc], CopyLength);
	}

	/*!
	\brief 폼모듈용 콜백함수 얻기
	\param operate : 마스크조건
	\param flag : 쓰일 마스크기호
	\return 폼모듈용 콜백함수
	*/
	virtual OnFormMethod GetFormMethod(maskoperate operate, maskflag flag)
	{
		switch(operate)
		{
		case maskoperate_all_print:
			if(flag == maskflag_no_write)
				return GetFormMethodSwitch<maskoperate_all_print, maskflag_no_write>();
			else if(flag == maskflag_write_1)
				return GetFormMethodSwitch<maskoperate_all_print, maskflag_write_1>();
			else if(flag == maskflag_write_0)
				return GetFormMethodSwitch<maskoperate_all_print, maskflag_write_0>();
			break;
		case maskoperate_no_print:
			return OnFormNoPrint;
		case maskoperate_print_by_1:
			if(flag == maskflag_no_write)
				return GetFormMethodSwitch<maskoperate_print_by_1, maskflag_no_write>();
			else if(flag == maskflag_write_1)
				return GetFormMethodSwitch<maskoperate_print_by_1, maskflag_write_1>();
			else if(flag == maskflag_write_0)
				return GetFormMethodSwitch<maskoperate_print_by_1, maskflag_write_0>();
			break;
		case maskoperate_print_by_0:
			if(flag == maskflag_no_write)
				return GetFormMethodSwitch<maskoperate_print_by_0, maskflag_no_write>();
			else if(flag == maskflag_write_1)
				return GetFormMethodSwitch<maskoperate_print_by_0, maskflag_write_1>();
			else if(flag == maskflag_write_0)
				return GetFormMethodSwitch<maskoperate_print_by_0, maskflag_write_0>();
			break;
		}
		return nullptr;
	}

protected:
	/// @cond SECTION_NAME
	template<maskoperate MASKOP, maskflag MASK, blendflag BLEND>
	global_func void OnForm(BxDrawForm& srcClass, const rect srcRect, pixel_dst* rowBuffer, const uint rowLength,
		const fint x, const fint y, const fint xVec, const fint yVec, const byte* blendSpriteTable, const byte* blendAlphaTable)
	{
		const pixel_dst* RowBufferEnd = rowBuffer + rowLength;
		const int AreaWidth = BxDrawForm::CalcTextureSize(srcClass.Width());
		const int AreaHeight = BxDrawForm::CalcTextureSize(srcClass.Height());
		const int SrcRowShift = BxDrawForm::CalcTextureLevel(AreaWidth);
		const int SrcWidthMask = AreaWidth - 1;
		const int SrcHeightMask = AreaHeight - 1;
		const pixel_src* SrcBits = ((BxImage*) &srcClass)->Bits + srcRect.l + srcRect.t * AreaWidth;
		fint XFocus = x + (AreaWidth << (16 + 6)), YFocus = y + (AreaHeight << (16 + 6));
		if(BLEND & blendflag_alpha)
		{
			pixel_src TempColor;
			switch(~(rowLength - 1) & 0x3F)
			while(rowBuffer < RowBufferEnd)
				CASE64(\
					if(MASKOP == maskoperate_all_print || (MASKOP == maskoperate_print_by_1 && (*rowBuffer & _MASK_BIT_)) || (MASKOP == maskoperate_print_by_0 && (~*rowBuffer & _MASK_BIT_)))\
					if(TempColor = SrcBits[(FtoI(XFocus) & SrcWidthMask) | ((FtoI(YFocus) & SrcHeightMask) << SrcRowShift)])\
					{\
						*rowBuffer = ((TempColor & _OPA_BIT_) == _OPA_BIT_)?\
							BxDrawGlobal::SpriteBlending(*rowBuffer, TempColor, blendSpriteTable)\
							: BxDrawGlobal::AlphaBlending(*rowBuffer, TempColor, blendAlphaTable);\
						if(MASK == maskflag_write_1) *rowBuffer |= _MASK_BIT_;\
						if(MASK == maskflag_write_0) *rowBuffer &= ~_MASK_BIT_;\
					}\
					++rowBuffer;\
					XFocus += xVec;\
					YFocus += yVec;)
		}
		else if(BLEND & blendflag_sprite)
		{
			pixel_src TempColor;
			switch(~(rowLength - 1) & 0x3F)
			while(rowBuffer < RowBufferEnd)
				CASE64(\
					if(MASKOP == maskoperate_all_print || (MASKOP == maskoperate_print_by_1 && (*rowBuffer & _MASK_BIT_)) || (MASKOP == maskoperate_print_by_0 && (~*rowBuffer & _MASK_BIT_)))\
					if(TempColor = SrcBits[(FtoI(XFocus) & SrcWidthMask) | ((FtoI(YFocus) & SrcHeightMask) << SrcRowShift)])\
					{\
						*rowBuffer = BxDrawGlobal::SpriteBlending(*rowBuffer, TempColor, blendSpriteTable);\
						if(MASK == maskflag_write_1) *rowBuffer |= _MASK_BIT_;\
						if(MASK == maskflag_write_0) *rowBuffer &= ~_MASK_BIT_;\
					}\
					++rowBuffer;\
					XFocus += xVec;\
					YFocus += yVec;)
		}
		else
		{
			pixel_src TempColor;
			switch(~(rowLength - 1) & 0x3F)
			while(rowBuffer < RowBufferEnd)
				CASE64(\
					if(MASKOP == maskoperate_all_print || (MASKOP == maskoperate_print_by_1 && (*rowBuffer & _MASK_BIT_)) || (MASKOP == maskoperate_print_by_0 && (~*rowBuffer & _MASK_BIT_)))\
					{\
						TempColor = SrcBits[(FtoI(XFocus) & SrcWidthMask) | ((FtoI(YFocus) & SrcHeightMask) << SrcRowShift)];\
						*rowBuffer = BxDrawGlobal::SpriteBlending(*rowBuffer, TempColor, blendSpriteTable);\
						if(MASK == maskflag_write_1) *rowBuffer |= _MASK_BIT_;\
						if(MASK == maskflag_write_0) *rowBuffer &= ~_MASK_BIT_;\
					}\
					++rowBuffer;\
					XFocus += xVec;\
					YFocus += yVec;)
		}
	}
	/// @endcond

	/// @cond SECTION_NAME
	template<maskoperate MASKOP, maskflag MASK>
	inline OnFormMethod GetFormMethodSwitch()
	{
		switch(Flag)
		{
		case blendflag_null: return OnForm<MASKOP, MASK, blendflag_null>;
		case blendflag_alpha: return OnForm<MASKOP, MASK, blendflag_alpha>;
		case blendflag_sprite: return OnForm<MASKOP, MASK, blendflag_sprite>;
		case blendflag_alpha | blendflag_sprite: return OnForm<MASKOP, MASK, (blendflag) 3>;
		}
		return nullptr;
	}
	/// @endcond

	/// @cond SECTION_NAME
	virtual bool _IsLoaded()
	{
		#ifdef __BX_OPENGL
			return (Bits != nullptr || FormGL != nullptr);
		#else
			return (Bits != nullptr);
		#endif
	}
	/// @endcond

	/// @cond SECTION_NAME
	#ifdef __BX_OPENGL
	virtual void _UpdateFormGL()
	{
		if(FormGL) return;
		const int WidthByMargin = BxCore::OpenGL2D::GetTextureMargin() + Width() + BxCore::OpenGL2D::GetTextureMargin();
		const int HeightByMargin = BxCore::OpenGL2D::GetTextureMargin() + Height() + BxCore::OpenGL2D::GetTextureMargin();
		color_a888* TextureImage = BxNew_Array(color_a888, WidthByMargin * HeightByMargin);
		BxCore::Util::MemSet(TextureImage, 0, sizeof(color_a888) * WidthByMargin * HeightByMargin);
		const int TextureRow = WidthByMargin;
		const int TextureMargin = BxCore::OpenGL2D::GetTextureMargin() + TextureRow * BxCore::OpenGL2D::GetTextureMargin();
		const int BitsRow = BxDrawForm::CalcTextureSize(Width());
		// 마진고려 복사
		for(int y = 0; y < Height(); ++y)
		for(int x = 0; x < Width(); ++x)
		{
			const pixel_src OnePixel = Bits[x + y * BitsRow];
			#ifdef __BX_PIXEL16
				TextureImage[TextureMargin + x + y * TextureRow] = BxDrawGlobal::ColorM555A444ToA888(OnePixel);
			#else
				TextureImage[TextureMargin + x + y * TextureRow] = OnePixel;
			#endif
		}
		BxDelete_Array(Bits);
		// 투명픽셀 보간
		if(BxCore::OpenGL2D::DoTextureInterpolation())
		{
			const color_a888 MaskPixel = 0x00F8F8F8;
			for(uint i = 0, ib = 0, iend = (uint) BxUtilGlobal::Max(1, BxCore::OpenGL2D::GetTextureMargin()); i < iend; ib = (++i & 1))
			for(int y = 0; y < HeightByMargin; ++y)
			{
				const bool IsYB = (0 < y), IsYE = (y < HeightByMargin - 1);
				for(int x = 0; x < WidthByMargin; ++x)
				{
					const int XM = x + 0, YM = (y + 0) * TextureRow;
					if(TextureImage[XM + YM]) continue;
					const bool IsXB = (0 < x), IsXE = (x < WidthByMargin - 1);
					const int XL = x - 1, XR = x + 1;
					const int YT = (y - 1) * TextureRow, YB = (y + 1) * TextureRow;
					int Check = 0;
					color_a888 OnePixelA = 0, Sample = 0;
					if(IsXB && IsYB && (Sample = TextureImage[XL + YT]) && ((Sample & 0xFF000000) || ((Sample & 1) == ib)) && ++Check) OnePixelA += (Sample & MaskPixel) >> 3;
					if(        IsYB && (Sample = TextureImage[XM + YT]) && ((Sample & 0xFF000000) || ((Sample & 1) == ib)) && ++Check) OnePixelA += (Sample & MaskPixel) >> 3;
					if(IsXE && IsYB && (Sample = TextureImage[XR + YT]) && ((Sample & 0xFF000000) || ((Sample & 1) == ib)) && ++Check) OnePixelA += (Sample & MaskPixel) >> 3;
					if(IsXB         && (Sample = TextureImage[XL + YM]) && ((Sample & 0xFF000000) || ((Sample & 1) == ib)) && ++Check) OnePixelA += (Sample & MaskPixel) >> 3;
					if(IsXE         && (Sample = TextureImage[XR + YM]) && ((Sample & 0xFF000000) || ((Sample & 1) == ib)) && ++Check) OnePixelA += (Sample & MaskPixel) >> 3;
					if(IsXB && IsYE && (Sample = TextureImage[XL + YB]) && ((Sample & 0xFF000000) || ((Sample & 1) == ib)) && ++Check) OnePixelA += (Sample & MaskPixel) >> 3;
					if(        IsYE && (Sample = TextureImage[XM + YB]) && ((Sample & 0xFF000000) || ((Sample & 1) == ib)) && ++Check) OnePixelA += (Sample & MaskPixel) >> 3;
					if(IsXE && IsYE && (Sample = TextureImage[XR + YB]) && ((Sample & 0xFF000000) || ((Sample & 1) == ib)) && ++Check) OnePixelA += (Sample & MaskPixel) >> 3;
					if(!Check) continue;
					const color_a888 OnePixelB = (const color_a888)
						(((((OnePixelA >> 16) & 0xFF) << 3) / Check) << 16) |
						(((((OnePixelA >> 8) & 0xFF) << 3) / Check) << 8) |
						(((((OnePixelA >> 0) & 0xFF) << 3) / Check) << 0);
					TextureImage[XM + YM] = (OnePixelB & 0xFFFFFFFE) | (~ib & 1);
				}
			}
		}
		FormGL = BxCore::OpenGL2D::MakeForm_TextureMaterial(TextureImage, WidthByMargin, HeightByMargin, TextureRow);
		BxDelete_Array(TextureImage);
	}
	#endif
	/// @endcond

	/// @cond SECTION_NAME
	bool _LoadBMP(const byte* Resource)
	{
		global_data const byte DevicePalette[3 * 16] = {
			0, 0, 0, 128, 0, 0, 0, 128, 0, 128, 128, 0,
			0, 0, 128, 128, 0, 128, 0, 128, 128, 128, 128, 128,
			192, 192, 192, 255, 0, 0, 0, 255, 0, 255, 255, 0,
			0, 0, 255, 255, 0, 255, 0, 255, 255, 255, 255, 255};

		byte* ResourcePtr = (byte*) Resource;
		if(BxUtilGlobal::LoadUint32(ResourcePtr) != *((uint*) "__BM"))
			return false;

		// FILEHEADER/INFOHEADER/PALETTE 로드
		color_bgra BitmapPalette[256] = {{0,},};
		bitmapfile BitmapFileHeader;
		bitmapinfo BitmapInfoHeader;
		BxCore::Util::MemCpy(&BitmapFileHeader, BxUtilGlobal::LoadType(ResourcePtr, sizeof(bitmapfile)), sizeof(bitmapfile));
		BxCore::Util::MemCpy(&BitmapInfoHeader, BxUtilGlobal::LoadType(ResourcePtr, sizeof(bitmapinfo)), sizeof(bitmapinfo));
		if(0 < BitmapInfoHeader.color_used)
			BxUtilGlobal::LoadBytes(ResourcePtr, BitmapPalette, BitmapInfoHeader.color_used * sizeof(color_bgra));
		else if(0 < BitmapFileHeader.offbits - 54)
		{
			BitmapInfoHeader.color_used = (BitmapFileHeader.offbits - 54) / sizeof(color_bgra);
			BxUtilGlobal::LoadBytes(ResourcePtr, BitmapPalette, BitmapFileHeader.offbits - 54);
		}

		// PALETTE 오류극복
		if(BitmapInfoHeader.color_used == 0 && BitmapInfoHeader.bitcount < 8)
		{
			switch(BitmapInfoHeader.bitcount)
			{
			case 1:
				BitmapInfoHeader.color_used = 2;
				BitmapPalette[0].r = 0;
				BitmapPalette[0].g = 0;
				BitmapPalette[0].b = 0;
				BitmapPalette[1].r = 255;
				BitmapPalette[1].g = 255;
				BitmapPalette[1].b = 255;
				break;
			case 4:
				{
					BitmapInfoHeader.color_used = 16;
					for(int i = 0, iend = BitmapInfoHeader.color_used; i < iend; ++i)
					{
						BitmapPalette[i].r = DevicePalette[i * 3 + 0];
						BitmapPalette[i].g = DevicePalette[i * 3 + 1];
						BitmapPalette[i].b = DevicePalette[i * 3 + 2];
					}
				}
				break;
			}
		}

		// PIXEL 로드
		if(0 < BitmapInfoHeader.width && 0 < BitmapInfoHeader.height)
		{
			const int RowLength = (((BitmapInfoHeader.width * BitmapInfoHeader.bitcount + 7) / 8 + 3) & ~3);
			byte* ImageRow = (byte*) Resource + 2 + BitmapFileHeader.offbits;
			const int AreaWidth = BxDrawForm::CalcTextureSize(BitmapInfoHeader.width);
			const int AreaHeight = BxDrawForm::CalcTextureSize(BitmapInfoHeader.height);
			Create(BitmapInfoHeader.width, BitmapInfoHeader.height, 0, 0);
			for(int y = Area.h - 1; 0 <= y; --y)
			{
				byte* ImageRowNext = ImageRow + RowLength;
				for(int x = 0, xbit = 0; x < Area.w; ++x)
				{
					const uint Color = BxUtilGlobal::LoadBits(ImageRow, xbit, BitmapInfoHeader.bitcount);
					switch(BitmapInfoHeader.bitcount)
					{
					case 16:
						if(Color == 0xF81F)
						{
							Flag = (blendflag)(Flag | blendflag_sprite);
							Bits[x + y * AreaWidth] = 0;
						}
						else Bits[x + y * AreaWidth] = BxDrawGlobal::Color565ToNativeSrc((color_565) Color);
						break;
					case 24:
						if(Color == 0xFF00FF)
						{
							Flag = (blendflag)(Flag | blendflag_sprite);
							Bits[x + y * AreaWidth] = 0;
						}
						else Bits[x + y * AreaWidth] = BxDrawGlobal::ColorX888ToNativeSrc((color_x888) Color);
						break;
					case 32:
						if((Color & 0xFF000000) == 0x00000000)
						{
							Flag = (blendflag)(Flag | blendflag_sprite);
							Bits[x + y * AreaWidth] = 0;
						}
						else
						{
							if((Color & 0xFF000000) != 0xFF000000) Flag = (blendflag)(Flag | blendflag_alpha);
							Bits[x + y * AreaWidth] = BxDrawGlobal::ColorA888ToNativeSrc((color_a888) Color);
						}
						break;
					default:
						{
							const color_bgra Palette = BitmapPalette[Color];
							if(Palette.r == 0xFF && Palette.g == 0x00 && Palette.b == 0xFF)
							{
								Flag = (blendflag)(Flag | blendflag_sprite);
								Bits[x + y * AreaWidth] = 0;
							}
							else Bits[x + y * AreaWidth] = BxDrawGlobal::ColorX888ToNativeSrc(BxDrawGlobal::RGB32(Palette.r, Palette.g, Palette.b));
						}
						break;
					}
				}
				ImageRow = ImageRowNext;
			}
			return true;
		}
		return false;
	}
	/// @endcond

	/// @cond SECTION_NAME
	bool _LoadB16(const byte* Resource)
	{
		byte* ResourcePtr = (byte*) Resource;
		if(BxUtilGlobal::LoadUint32(ResourcePtr) != *((uint*) "B16h"))
			return false;
		BxUtilGlobal::LoadUint32(ResourcePtr); // skip HASH
		Area.w = BxUtilGlobal::LoadUint16(ResourcePtr);
		Area.h = BxUtilGlobal::LoadUint16(ResourcePtr);
		ExpressAreaW.Reset(BxInteger(0) + (int) Area.w);
		ExpressAreaH.Reset(BxInteger(0) + (int) Area.h);
		Area.hx = BxUtilGlobal::LoadInt16(ResourcePtr);
		Area.hy = BxUtilGlobal::LoadInt16(ResourcePtr);
		const int AreaWidth = BxDrawForm::CalcTextureSize(Area.w);
		const int AreaHeight = BxDrawForm::CalcTextureSize(Area.h);
		Create(Area.w, Area.h, Area.hx, Area.hy);
		color_m555_a444* ImagePtr = (color_m555_a444*) ResourcePtr;
		for(int i = 0, iend = Area.w * Area.h; i < iend && Flag != (blendflag_alpha | blendflag_sprite); ++i)
		{
			if(!ImagePtr[i]) Flag = (blendflag)(Flag | blendflag_sprite);
			else if(!(ImagePtr[i] & 0x8000)) Flag = (blendflag)(Flag | blendflag_alpha);
		}
		for(int y = 0; y < Area.h; ++y)
		for(int x = 0; x < Area.w; ++x)
			Bits[x + AreaWidth * y] = BxDrawGlobal::ColorM555A444ToNativeSrc(ImagePtr[x + Area.w * y]);
		return true;
	}
	/// @endcond

	/// @cond SECTION_NAME
	bool _LoadB32(const byte* Resource)
	{
		byte* ResourcePtr = (byte*) Resource;
		if(BxUtilGlobal::LoadUint32(ResourcePtr) != *((uint*) "B32h"))
			return false;
		BxUtilGlobal::LoadUint32(ResourcePtr); // skip HASH
		Area.w = BxUtilGlobal::LoadUint16(ResourcePtr);
		Area.h = BxUtilGlobal::LoadUint16(ResourcePtr);
		ExpressAreaW.Reset(BxInteger(0) + (int) Area.w);
		ExpressAreaH.Reset(BxInteger(0) + (int) Area.h);
		Area.hx = BxUtilGlobal::LoadInt16(ResourcePtr);
		Area.hy = BxUtilGlobal::LoadInt16(ResourcePtr);
		const int AreaWidth = BxDrawForm::CalcTextureSize(Area.w);
		const int AreaHeight = BxDrawForm::CalcTextureSize(Area.h);
		Create(Area.w, Area.h, Area.hx, Area.hy);
		color_a888* ImagePtr = (color_a888*) ResourcePtr;
		for(int i = 0, iend = Area.w * Area.h; i < iend && Flag != (blendflag_alpha | blendflag_sprite); ++i)
		{
			if(!ImagePtr[i]) Flag = (blendflag)(Flag | blendflag_sprite);
			else if((ImagePtr[i] & 0xFF000000) != 0xFF000000) Flag = (blendflag)(Flag | blendflag_alpha);
		}
		for(int y = 0; y < Area.h; ++y)
		for(int x = 0; x < Area.w; ++x)
			Bits[x + AreaWidth * y] = BxDrawGlobal::ColorA888ToNativeSrc(ImagePtr[x + Area.w * y]);
		return true;
	}
	/// @endcond

	/// @cond SECTION_NAME
	bool _LoadPNG(const byte* Resource)
	{
		PngToBmp PngDecoder;
		const byte* Bmp = PngDecoder.GetResource(Resource, 1 < ScaleNumber());
		const byte* ScaledBmp = BxCore::AddOn::HQXToBMP(Bmp, ScaleNumber());
		bool Result = _LoadBMP((ScaledBmp)? ScaledBmp : Bmp);
		BxCore::AddOn::Release(ScaledBmp);
		return Result;
	}
	/// @endcond

	/// @cond SECTION_NAME
	bool _LoadJPG(const byte* Resource)
	{
		const byte* Bmp = BxCore::AddOn::JPGToBMP(Resource);
		const byte* ScaledBmp = BxCore::AddOn::HQXToBMP(Bmp, ScaleNumber());
		bool Result = _LoadBMP((ScaledBmp)? ScaledBmp : Bmp);
		BxCore::AddOn::Release(ScaledBmp);
		BxCore::AddOn::Release(Bmp);
		return Result;
	}
	/// @endcond

	/// @cond SECTION_NAME
	bool _LoadGIF(const byte* Resource, const int Length)
	{
		int NumPage = 1;
		const byte* Bmp = BxCore::AddOn::GIFToBMP(Resource, Length, NumPage);
		const byte* ScaledBmp = BxCore::AddOn::HQXToBMP(Bmp, ScaleNumber());
		bool Result = _LoadBMP((ScaledBmp)? ScaledBmp : Bmp);
		BxCore::AddOn::Release(ScaledBmp);
		BxCore::AddOn::Release(Bmp);
		Page = NumPage;
		return Result;
	}
	/// @endcond

public:
	/// @cond SECTION_NAME
	class PngToBmp
	{
	public:
		PngToBmp(bool autodelete = true)
		{
			AutoDelete = autodelete;
			IsFinalBlock = false;
			BlockMode = (char) 0xFF;
			LiteralLength = 0;
			BMPData = nullptr;
			BMPOffset = 0;
			BMPSize = 0;

			global_data const int _LengthBase[29] = {
				 3,   4,   5,   6,   7,   8,   9,  10,  11,  13,
				15,  17,  19,  23,  27,  31,  35,  43,  51,  59,
				67,  83,  99, 115, 131, 163, 195, 227, 258};
			global_data const int _LengthExtra[29] = {
				0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
				1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
				4, 4, 4, 4, 5, 5, 5, 5, 0};
			global_data const int _DistanceBase[30] = {
				   1,     2,     3,     4,     5,     7,     9,    13,    17,    25,
				  33,    49,    65,    97,   129,   193,   257,   385,   513,   769,
				1025,  1537,  2049,  3073,  4097,  6145,  8193, 12289, 16385, 24577};
			global_data const int _DistanceExtra[30] = {
				0,  0,  0,  0,  1,  1,  2,  2,  3,  3,
				4,  4,  5,  5,  6,  6,  7,  7,  8,  8,
				9,  9, 10, 10, 11, 11, 12, 12, 13, 13};
			global_data const int _PngLengthOrder[19] = {
				16, 17, 18,  0,  8,  7,  9,  6, 10,  5,
				11,  4, 12,  3, 13,  2, 14,  1, 15};
			BxCore::Util::MemCpy((int*) LengthBase, _LengthBase, sizeof(int) * 29);
			BxCore::Util::MemCpy((int*) LengthExtra, _LengthExtra, sizeof(int) * 29);
			BxCore::Util::MemCpy((int*) DistanceBase, _DistanceBase, sizeof(int) * 30);
			BxCore::Util::MemCpy((int*) DistanceExtra, _DistanceExtra, sizeof(int) * 30);
			BxCore::Util::MemCpy((int*) PngLengthOrder, _PngLengthOrder, sizeof(int) * 19);
		}
		virtual ~PngToBmp()
		{
			if(AutoDelete)
				DeleteResource();
		}

		// PNG메모리를 BMP메모리로 변환함
		byte* GetResource(const byte* pngsource, bool ismust32 = false)
		{
			DeleteResource();

			// 작업변수 초기화
			_Buf = pngsource;
			_Off = 0;
			LZPos = 0;
			LZCopyPos = 0;
			LZCopyCount = 0;

			// 청크정보 수집
			uint ImageWidth = 0;
			uint ImageHeight = 0;
			uint ImageDepth = 0;
			uint ImageColorType = 0;
			byte BMPPalette[3 * 256];
			byte BMPPaletteAlpha[256];
			ushort BytePerPixel = 0;
			uint BMPDataWidth = 0;
			byte* DataPtr = nullptr;
			{
				const byte* PNGNext = pngsource + 4;
				uint ChunkName = 0;

				// IHDR청크
				while(ChunkName != 0x49484452) // IHDR
				{
					_Buf = PNGNext + 4;
					PNGNext += GetUInt32() + 12;
					ChunkName = GetUInt32();
					if(ChunkName == 0x49454E44) // IEND
					{
						BxASSERT("BxImage<PNG에 IHDR청크가 없습니다>", false);
						return nullptr;
					}
				}

				ImageWidth = (uint) GetUInt32();
				ImageHeight = (uint) GetUInt32();
				ImageDepth = (uint) GetByte();
				ImageColorType = (uint) GetByte();
				GetByte(); GetByte(); // Compression Method, Filter Method 생략

				// 인터레이스를 지원하지 않는다
				if(0 < GetByte())
				{
					BxASSERT("BxImage<인터레이스가 들어간 PNG는 지원하지 않습니다>", false);
					return nullptr;
				}

				// PLTE청크
				uint PaletteSize = 0;
				while(ChunkName != 0x504C5445) // PLTE
				{
					_Buf = PNGNext + 4;
					PNGNext += (PaletteSize = GetUInt32()) + 12;
					ChunkName = GetUInt32();
					if(ChunkName == 0x49454E44) // IEND
					{
						PNGNext = (byte*) pngsource + 4;
						break;
					}
				}
				if(0 < PaletteSize)
				for(uint p = 0; p < PaletteSize; p++)
					BMPPalette[p] = GetByte();

				// tRNS청크
				uint TransparencySize = 0;
				while(ChunkName != 0x74524E53) // tRNS
				{
					_Buf = PNGNext + 4;
					PNGNext += (TransparencySize = GetUInt32()) + 12;
					ChunkName = GetUInt32();
					if(ChunkName == 0x49454E44) // IEND
					{
						PNGNext = (byte*) pngsource + 4;
						break;
					}
				}
				if(0 < TransparencySize)
				for(uint a = 0; a < TransparencySize; a++)
					BMPPaletteAlpha[a] = GetByte();

				// 비트맵구성
				BytePerPixel = (ImageColorType == 4 || ImageColorType == 6 || (ImageColorType == 3 && 0 < TransparencySize) || ismust32)? 4 : 3;
				BMPDataWidth = (ImageWidth * BytePerPixel + 3) & ~3;
				BMPSize = 56 + ImageHeight * BMPDataWidth;
				BMPData = BxNew_Array(byte, BMPSize);
				BxCore::Util::MemSet(BMPData, 0xFF, BMPSize);
				SetBMPInfo(BMPData, ImageWidth, ImageHeight, BytePerPixel);

				// IDAT청크 조사
				uint DataSize = 0;
				bool IsMultiData = false;
				do
				{
					_Buf = PNGNext + 4;
					const uint TempSize = GetUInt32();
					PNGNext += TempSize + 12;
					ChunkName = GetUInt32();
					if(ChunkName == 0x49444154) // IDAT
					{
						if(!DataPtr) DataPtr = (byte*) _Buf;
						else IsMultiData = true;
						DataSize += TempSize;
					}
				}
				while(ChunkName != 0x49454E44); // IEND
				if(!DataPtr)
				{
					BxASSERT("BxImage<PNG에 IDAT청크가 없습니다>", false);
					return nullptr;
				}

				// IDAT청크 병합(IDAT가 복수개일 경우만)
				if(IsMultiData)
				{
					DataPtr = (byte*) BxCore::Util::Alloc(DataSize);
					byte* DataFocus = DataPtr;
					PNGNext = (byte*) pngsource + 4;
					do
					{
						_Buf = PNGNext + 4;
						const uint TempSize = GetUInt32();
						PNGNext += TempSize + 12;
						ChunkName = GetUInt32();
						if(ChunkName == 0x49444154) // IDAT
						{
							BxCore::Util::MemCpy(DataFocus, _Buf, TempSize);
							DataFocus += TempSize;
						}
					}
					while(ChunkName != 0x49454E44); // IEND
				}
				_Buf = DataPtr;
				if(!IsMultiData)
					DataPtr = nullptr;
			}

			// 디코딩된 픽셀정보 임시저장소
			uint RowBufferWidth = 0, FilterOffset = 0;
			switch(ImageColorType)
			{
			case 0: // 흑백
			case 3: // 팔레트
				RowBufferWidth = ((ImageWidth * ImageDepth) + 7) / 8;
				FilterOffset = 1;
				break;
			case 4: // 투명흑백
				RowBufferWidth = 2 * ImageWidth * ImageDepth / 8;
				FilterOffset = 2 * ImageDepth / 8;
				break;
			case 2: // RGB
				RowBufferWidth = 3 * ImageWidth * ImageDepth / 8;
				FilterOffset = 3 * ImageDepth / 8;
				break;
			case 6: // 투명RGB
				RowBufferWidth = 4 * ImageWidth * ImageDepth / 8;
				FilterOffset = 4 * ImageDepth / 8;
				break;
			}

			// 열버퍼생성
			byte* RowBuffers = (byte*) BxCore::Util::Alloc(RowBufferWidth * 2);
			byte* _ref_ RowBufferA = RowBuffers;
			byte* _ref_ RowBufferB = RowBuffers + RowBufferWidth;
			BxCore::Util::MemSet(RowBufferA, 0x00, RowBufferWidth);
			BxCore::Util::MemSet(RowBufferB, 0x00, RowBufferWidth);

			GetByte(); // Compression Method 생략
			GetByte(); // Compression Level, Preset Dictionary 생략
			// 첫 블럭헤더 디코딩
			DecodeBlockHeader();

			// 행단위 디코딩
			for(uint y = 0; y < ImageHeight; ++y)
			{
				const byte Filter = DecodeByte();
				for(uint x = 0; x < RowBufferWidth; ++x)
					RowBufferA[x] = DecodeByte();

				// 필터링
				switch(Filter)
				{
				case 1: // Sub필터
					for(uint Col = FilterOffset; Col < RowBufferWidth; ++Col)
						RowBufferA[Col] += RowBufferA[Col - FilterOffset];
					break;
				case 2: // Up필터
					for(uint Col = 0; Col < RowBufferWidth; ++Col)
						RowBufferA[Col] += RowBufferB[Col];
					break;
				case 3: // Average필터
					for(uint Col = 0; Col < FilterOffset; ++Col)
						RowBufferA[Col] += RowBufferB[Col] / 2;
					for(uint Col = FilterOffset; Col < RowBufferWidth; ++Col)
						RowBufferA[Col] += (RowBufferA[Col - FilterOffset] + RowBufferB[Col]) / 2;
					break;
				case 4: // Paeth필터
					for(uint Col = 0; Col < FilterOffset; ++Col)
						RowBufferA[Col] += RowBufferB[Col];
					for(uint Col = FilterOffset; Col < RowBufferWidth; ++Col)
					{
						// 죄측, 윗열, 윗좌측의 값들의 평균값에 가까운 필터를 사용
						// 전체압축율에 손실을 입힐 우려가 있는 필터를 배제하기 위함
						const byte Left = RowBufferA[Col - FilterOffset];
						const byte Above = RowBufferB[Col];
						const byte Aboveleft = RowBufferB[Col - FilterOffset];
						int PaethA = Above - Aboveleft;
						int PaethB = Left - Aboveleft;
						int PaethC = PaethA + PaethB;
						PaethA *= PaethA;
						PaethB *= PaethB;
						PaethC *= PaethC;
						RowBufferA[Col] += (PaethA <= PaethB && PaethA <= PaethC)? Left
							: ((PaethB <= PaethC)? Above : Aboveleft);
					}
					break;
				}

				// 이미지쓰기
				uint ImageOffset = 56 + (ImageHeight - y - 1) * BMPDataWidth;
				switch(ImageColorType)
				{
				case 0: // 흑백
					for(uint x = 0, xend = ImageDepth * ImageWidth; x < xend; x += ImageDepth, ImageOffset += BytePerPixel)
					{
						const byte* Bytes = &RowBufferA[x >> 3];
						const uhuge Union
							= ((Bytes[0] & ox00000000000000FF) << 0)
							| ((Bytes[1] & ox00000000000000FF) << 8)
							| ((Bytes[2] & ox00000000000000FF) << 16)
							| ((Bytes[3] & ox00000000000000FF) << 24)
							| ((Bytes[4] & ox00000000000000FF) << 32);
						const uint GrayLevel = (uint) (Union >> (x & 7)) & ~(0xFFFFFFFF << ImageDepth);
						const byte ColorValue = (byte) (GrayLevel * 255 / ((1 << ImageDepth) - 1));
						BMPData[ImageOffset] = ColorValue;
						BMPData[ImageOffset + 1] = ColorValue;
						BMPData[ImageOffset + 2] = ColorValue;
					}
					break;
				case 3: // 팔레트
					for(uint x = 0, xend = ImageDepth * ImageWidth; x < xend; x += ImageDepth, ImageOffset += BytePerPixel)
					{
						const byte* Bytes = &RowBufferA[x >> 3];
						const uhuge Union
							= ((Bytes[0] & ox00000000000000FF) << 0)
							| ((Bytes[1] & ox00000000000000FF) << 8)
							| ((Bytes[2] & ox00000000000000FF) << 16)
							| ((Bytes[3] & ox00000000000000FF) << 24)
							| ((Bytes[4] & ox00000000000000FF) << 32);
						const uint AlphaOffset = (uint) (Union >> (x & 7)) & ~(0xFFFFFFFF << ImageDepth);
						const uint PaletteOffset = AlphaOffset * 3;
						BMPData[ImageOffset] = BMPPalette[PaletteOffset + 2];
						BMPData[ImageOffset + 1] = BMPPalette[PaletteOffset + 1];
						BMPData[ImageOffset + 2] = BMPPalette[PaletteOffset];
						if(BytePerPixel == 4)
						{
							if(BMPPaletteAlpha)
								BMPData[ImageOffset + 3] = BMPPaletteAlpha[AlphaOffset];
							else BMPData[ImageOffset + 3] = 0xFF;
						}
					}
					break;
				case 4: // 투명흑백
					for(uint x = 0, xend = ImageWidth * 2; x < xend; x += 2)
					{
						BMPData[ImageOffset++] = RowBufferA[x + 0];
						BMPData[ImageOffset++] = RowBufferA[x + 0];
						BMPData[ImageOffset++] = RowBufferA[x + 0];
						if(BytePerPixel == 4)
							BMPData[ImageOffset++] = RowBufferA[x + 1];
					}
					break;
				case 2: // RGB
					for(uint x = 0, xend = ImageWidth * 3; x < xend; x += 3)
					{
						BMPData[ImageOffset++] = RowBufferA[x + 2];
						BMPData[ImageOffset++] = RowBufferA[x + 1];
						BMPData[ImageOffset++] = RowBufferA[x + 0];
						if(BytePerPixel == 4)
							BMPData[ImageOffset++] = 0;
					}
					break;
				case 6: // 투명RGB
					for(uint x = 0, xend = ImageWidth * 4; x < xend; x += 4)
					{
						BMPData[ImageOffset++] = RowBufferA[x + 2];
						BMPData[ImageOffset++] = RowBufferA[x + 1];
						BMPData[ImageOffset++] = RowBufferA[x + 0];
						if(BytePerPixel == 4)
							BMPData[ImageOffset++] = RowBufferA[x + 3];
					}
					break;
				}

				// 열버퍼교환
				byte* RowBufferTemp = RowBufferA;
				RowBufferA = RowBufferB;
				RowBufferB = RowBufferTemp;
			}
			RowBuffers = (byte*) BxCore::Util::Free(RowBuffers);
			DataPtr = (byte*) BxCore::Util::Free(DataPtr);
			return BMPData;
		}

		// 비트맵의 사이즈
		inline uint GetResourceSize()
		{
			return BMPSize;
		}

		// 비트맵리소스를 메모리상에서 해제
		inline void DeleteResource()
		{
			BxDelete_Array(BMPData);
			BMPData = nullptr;
		}

	protected:
		bool AutoDelete; // 소멸시 생산된 비트맵리소스의 삭제여부
		bool IsFinalBlock; // IDAT의 블럭헤더 디코딩중 현재 블럭이 마지막인지를 나타냄
		char BlockMode; // IDAT의 블럭헤더 디코딩중 현재 블럭이 어떠한 블럭인지를 나타냄
		int LiteralLength; // 비압축블럭의 사이즈
		byte* BMPData; // 생산된 비트맵리소스
		uint BMPOffset; // 비트맵의 바이트단위 오프셋
		uint BMPSize; // 디코딩된 비트맵의 사이즈

	protected:
		const byte* _Buf;
		uint _Off;
		byte LZWindow[32768];
		uint LZPos;
		uint LZCopyPos;
		uint LZCopyCount;
		int MaxCode[16 * 3]; // [3][15]
		int MinCode[16 * 3]; // [3][15]
		int ValueArray[16 * 3]; // [3][15]
		int HuffValues[512 * 3]; // [3][288]
		int LengthBase[29];
		int LengthExtra[29];
		int DistanceBase[30];
		int DistanceExtra[30];
		int PngLengthOrder[19];

	protected:
		// 1비트를 읽음
		inline uint GetBit()
		{
			const uint RValue = (*_Buf >> _Off) & 1;
			const int Value = _Off + 1;
			_Buf += Value >> 3;
			_Off = Value & 7;
			return RValue;
		}

		// len비트를 읽음, len허용범위 : 0-32
		inline uint GetBits(const uint len)
		{
			const uhuge Union
				= ((_Buf[0] & ox00000000000000FF) << 0)
				| ((_Buf[1] & ox00000000000000FF) << 8)
				| ((_Buf[2] & ox00000000000000FF) << 16)
				| ((_Buf[3] & ox00000000000000FF) << 24)
				| ((_Buf[4] & ox00000000000000FF) << 32);
			const uint RValue = (uint) (Union >> _Off) & ~(0xFFFFFFFF << len);
			const int Value = _Off + len;
			_Buf += Value >> 3;
			_Off = Value & 7;
			return RValue;
		}

		// 1바이트를 읽음, GetBits()로 읽다 남은것이 있다면 해당 바이트 건너뜀
		inline byte GetByte()
		{
			// 비트단위로 읽던것은 넘어감
			_Buf += (_Off != 0);
			_Off = 0;
			return *(_Buf++);
		}

		// 4바이트를 읽음
		inline uint GetUInt32()
		{
			// 비트단위로 읽던것은 넘어감
			_Buf += (_Off != 0);
			_Off = 0;
			const uint RValue =
				((_Buf[0] & 0xFF) << 24) |
				((_Buf[1] & 0xFF) << 16) |
				((_Buf[2] & 0xFF) << 8) |
				((_Buf[3] & 0xFF) << 0);
			_Buf += 4;
			return RValue;
		}

		inline byte DecodeLiteralByte()
		{
			if(LiteralLength == 0)
			{
				if(!IsFinalBlock)
					DecodeBlockHeader();
				return DecodeByte();
			}
			--LiteralLength;
			return GetByte();
		}

		// 허프만방식으로 읽음
		inline byte DecodeByte()
		{
			if(BlockMode == 0)
				return DecodeLiteralByte();
			return DecodeCompressedByte();
		}

		// 허프만테이블에서 값을 찾음
		int Decode(const uint tablenum)
		{
			int CodeLength = 0, Code = GetBit();
			const int* MaxCodeFocus = &MaxCode[tablenum << 4];
			for(CodeLength = 0; MaxCodeFocus[CodeLength] < Code && CodeLength < 15; ++CodeLength)
				Code = (Code << 1) | GetBit(); // 1비트씩 거꾸로 읽음
			return HuffValues[(tablenum << 9) | (ValueArray[(tablenum << 4) | CodeLength] + (Code - MinCode[(tablenum << 4) | CodeLength]))];
		}

		byte DecodeCompressedByte()
		{
			// LZ윈도우의 복사거리동안 디코딩은 쉼
			if(0 < LZCopyCount)
			{
				byte Value8 = LZWindow[LZCopyPos];
				LZWindow[LZPos] = Value8;
				--LZCopyCount;
				LZCopyPos = (LZCopyPos + 1) & 0x7FFF;
				LZPos = (LZPos + 1) & 0x7FFF;
				return Value8;
			}

			// 허프만트리에서 하나의 값을 얻음
			const uint ValueA = Decode(0);
			if(ValueA == 256)
			{
				if(!IsFinalBlock)
					DecodeBlockHeader();
				return DecodeByte();
			}
			else if(ValueA < 256) // LZ윈도우상의 기호토큰인 경우
			{
				LZWindow[LZPos] = (byte) ValueA;
				LZPos = (LZPos + 1) & 0x7FFF;
				return (byte) ValueA;
			}

			// LZ윈도우상의 구를 이루는 토큰인 경우
			const uint Length = LengthBase[ValueA - 257] + GetBits(LengthExtra[ValueA - 257]);
			const uint ValueB = Decode(1);
			const uint Distance = DistanceBase[ValueB] + GetBits(DistanceExtra[ValueB]);
			LZCopyPos = (32768 + LZPos - Distance) & 0x7FFF;
			LZCopyCount = Length;
			return DecodeCompressedByte();
		}

		// 블럭헤더의 Length를 읽음
		void DecodeLengths(uint tablenum, uint* lengths, uint lengthcount)
		{
			// 동적허프만테이블인 경우 이미지정보값과 LZ윈도우거리값의 배열이
			// 따로 기록되어 있으니 디코딩해야 하며 RLE압축방식을 쓴다
			uint Index = 0;
			while(Index < lengthcount)
			{
				uint Command = Decode(tablenum);
				if(Command < 16) lengths[Index++] = Command;
				else
				{
					uint Count = 0;
					switch(Command)
					{
					case 16: Count = GetBits(2) + 3; break; // 카운트만큼 이전과 같음
					case 17: Count = GetBits(3) + 3; break; // 카운트만큼 0값
					case 18: Count = GetBits(7) + 11; break; // 카운트만큼 0값
					}
					for(uint i = 0; i < Count; ++i)
					{
						if(Command == 16)
						{
							const uint Index2 = Index - 1;
							lengths[Index++] = lengths[Index2];
						}
						else lengths[Index++] = 0;
					}
				}
			}
		}

		// IDAT의 블럭헤더를 디코딩함
		void DecodeBlockHeader()
		{
			// 압축블럭은 이미지 행단위가 아니라 행속에서의 효율단위이다
			IsFinalBlock = (GetBit() != 0);
			switch(GetBits(2))
			{
			case 0: // 비압축블럭
				{
					BlockMode = 0;
					LiteralLength = GetByte();
					LiteralLength |= ((int) GetByte()) << 8;
					GetByte(); GetByte(); // Not Length 생략
				}
				break;
			case 1: // 고정허프만테이블
				if(BlockMode != 1)
				{
					BlockMode = 1;
					BuildFixedTable();
				}
				break;
			case 2: // 동적허프만테이블
				{
					BlockMode = 2;
					const uint LiteralsNum = GetBits(5) + 257;
					const uint DistancesNum = GetBits(5) + 1;
					const uint LengthsNum = GetBits(4) + 4;
					// Lengths 빌드
					uint Lengths[19];
					BxCore::Util::MemSet(Lengths, 0, sizeof(uint) * 19);
					for(uint i = 0; i < LengthsNum; ++i)
						Lengths[PngLengthOrder[i]] = GetBits(3);
					BuildTable(2, 19, Lengths);
					// Literals/Distances 빌드
					uint Literals[288], Distances[32];
					DecodeLengths(2, Literals, LiteralsNum);
					DecodeLengths(2, Distances, DistancesNum);
					BuildTable(0, LiteralsNum, Literals);
					BuildTable(1, DistancesNum, Distances);
				}
				break;
			}
		}

		// 고정허프만테이블 구성
		void BuildFixedTable()
		{
			for(int j = 0; j < 2; j++)
			for(int i = 0; i < 15; ++i)
			{
				MaxCode[(j << 4) | i] = -1;
				MinCode[(j << 4) | i] = 0x0FFFFFFF;
				ValueArray[(j << 4) | i] = 0;
			}

			MaxCode[(0 << 4) | 6] = 23, MaxCode[(0 << 4) | 7] = 199, MaxCode[(0 << 4) | 8] = 511;
			MinCode[(0 << 4) | 6] = 0, MinCode[(0 << 4) | 7] = 48, MinCode[(0 << 4) | 8] = 400;
			ValueArray[(0 << 4) | 6] = 0, ValueArray[(0 << 4) | 7] = 24, ValueArray[(0 << 4) | 8] = 176;
			int valuesfocus = 0;
			for(ushort f = 256; f <= 279; ++f) HuffValues[(0 << 9) | valuesfocus++] = f;
			for(ushort f = 0; f <= 143; ++f) HuffValues[(0 << 9) | valuesfocus++] = f;
			for(ushort f = 280; f <= 287; ++f) HuffValues[(0 << 9) | valuesfocus++] = f;
			for(ushort f = 144; f <= 255; ++f) HuffValues[(0 << 9) | valuesfocus++] = f;

			MaxCode[(1 << 4) | 4] = 31, MinCode[(1 << 4) | 4] = 0, ValueArray[(1 << 4) | 4] = 0;
			valuesfocus = 0;
			for(ushort f = 0; f <= 31; ++f) HuffValues[(1 << 9) | valuesfocus++] = f;
		}

		// 동적허프만테이블 구성
		void BuildTable(uint tablenum, uint vcount, const uint* codelengths)
		{
			uint LengthArray[288];
			// 배열번호는 그 자체로 최종적으로 디코딩시 얻는 이미지정보등의 값이며
			// Length는 그 값을 표현하기 위한 비트수이다
			for(uint i = 0; i < vcount; ++i)
			{
				HuffValues[(tablenum << 9) | i] = i;
				LengthArray[i] = codelengths[i];
			}

			// 배열의 뒤로 갈수록 비트수가 많아지게 정렬한다
			for(uint i = 0; i < vcount - 1; ++i)
			{
				uint MinIndex = i;
				for(uint j = i + 1; j < vcount; ++j)
					if(LengthArray[j] < LengthArray[MinIndex]
						|| (LengthArray[j] == LengthArray[MinIndex] && HuffValues[(tablenum << 9) | j] < HuffValues[(tablenum << 9) | MinIndex]))
							MinIndex = j;
				if(MinIndex != i)
				{
					const int Temp1 = HuffValues[(tablenum << 9) | MinIndex];
					HuffValues[(tablenum << 9) | MinIndex] = HuffValues[(tablenum << 9) | i];
					HuffValues[(tablenum << 9) | i] = Temp1;
					const uint Temp2 = LengthArray[MinIndex];
					LengthArray[MinIndex] = LengthArray[i];
					LengthArray[i] = Temp2;
				}
			}

			ushort HuffCodes[288];
			// 해당비트수로 표현할 수 있는 최소수부터 1씩 올라가며 차례차례 기록한다
			uint LastLen = 0, Code = 0;
			for(uint i = 0; i < vcount; ++i)
			{
				while(LastLen < LengthArray[i])
				{
					++LastLen;
					Code <<= 1;
				}
				if(LastLen != 0)
					HuffCodes[i] = (ushort) Code++;
			}

			// 비트수는 보통 5, 7, 8, 9만을 쓰기 때문에 초기화함
			for(uint i = 0; i < 15; ++i)
			{
				MaxCode[(tablenum << 4) | i] = -1;
				MinCode[(tablenum << 4) | i] = 0x0FFFFFFF;
				ValueArray[(tablenum << 4) | i] = 0;
			}

			// 비트수에 따른 최소값과 최대값, 이미지정보기본값을 저장한다
			uint LastLength = 0;
			for(uint i = 0; i < vcount; ++i)
			{
				if(LastLength < LengthArray[i])
				{
					LastLength = LengthArray[i];
					ValueArray[(tablenum << 4) | (LastLength - 1)] = (ushort) i;
					MinCode[(tablenum << 4) | (LastLength - 1)] = HuffCodes[i];
				}
				if(0 < LastLength)
					MaxCode[(tablenum << 4) | (LastLength - 1)] = HuffCodes[i];
			}
		}

		// BMP 헤더정보 채우기
		void SetBMPInfo(byte* bmp, uint w, uint h, ushort byteperpixel)
		{
			BMPOffset = 0;
			// 비트맵정보구성
			SetBMPUInt32(bmp, *((uint*) "__BM"));
			SetBMPUInt32(bmp, 54 + ((w * byteperpixel + 3) & ~3) * h);
			SetBMPUInt16(bmp, 0);
			SetBMPUInt16(bmp, 0);
			SetBMPUInt32(bmp, 54);
			SetBMPUInt32(bmp, 40);
			SetBMPUInt32(bmp, w);
			SetBMPUInt32(bmp, h);
			SetBMPUInt16(bmp, 1);
			SetBMPUInt16(bmp, 8 * byteperpixel);
			SetBMPUInt32(bmp, 0);
			SetBMPUInt32(bmp, ((w * byteperpixel + 3) & ~3) * h);
			SetBMPUInt32(bmp, 3780);
			SetBMPUInt32(bmp, 3780);
			SetBMPUInt32(bmp, 0);
			SetBMPUInt32(bmp, 0);
		}

		void SetBMPUInt16(byte* bmp, ushort value)
		{
			bmp[BMPOffset] = (byte) (value & 0xFF);
			bmp[BMPOffset + 1] = (byte) ((value >> 8) & 0xFF);
			BMPOffset += 2;
		}

		void SetBMPUInt32(byte* bmp, uint value)
		{
			bmp[BMPOffset] = (byte) (value & 0xFF);
			bmp[BMPOffset + 1] = (byte) ((value >> 8) & 0xFF);
			bmp[BMPOffset + 2] = (byte) ((value >> 16) & 0xFF);
			bmp[BMPOffset + 3] = (byte) ((value >> 24) & 0xFF);
			BMPOffset += 4;
		}
	};
	/// @endcond
};
