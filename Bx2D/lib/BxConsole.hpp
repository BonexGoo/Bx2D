#pragma once
#include "BxImage.hpp"
#include "BxVarMap.hpp"
#include "BxVarVector.hpp"

//! \brief 콘솔 관리
class BxConsole
{
public:
    void setcolor(int front16, int back16)
    {
        LastColorF = front16;
        LastColorB = back16;
    }

    void gotoxy(int x, int y)
    {
        LastPosX = Min(Max(0, x), 30 - 1);
        LastPosY = Min(Max(0, y), 20 - 1);
    }

    void printf(string map, const BxArgument& args = BxArgument::zero())
    {
        string Str = BxCore::Util::Print(map, args);
        while(*Str)
        {
            MapColorF[LastPosY][LastPosX] = LastColorF;
            MapColorB[LastPosY][LastPosX] = LastColorB;
            MapText[LastPosY][LastPosX][0] = *(Str++);
            MapText[LastPosY][LastPosX][1] = (Str[-1] & 0x80)? *(Str++) : '\0';
            if(30 - 1 < ++LastPosX)
            {
                LastPosX = 0;
                if(20 - 1 < ++LastPosY)
                    LastPosY = 0;
            }
        }
    }

    void flush(BxDraw& draw)
    {
        char Chars[3] = {'\0', '\0', '\0'};
        for(int y = 0; y < 20; ++y)
        for(int x = 0; x < 30; ++x)
        {
            BxImage* CurBack = Back.Access(MapColorB[y][x]);
            if(CurBack && CurBack->IsExist())
            {
                const int PosX = 32 * x + 32 - CurBack->Width();
                const int PosY = 32 * y + 32 - CurBack->Height();
                draw.Area(PosX, PosY, FORM(CurBack)); // 배경이미지
            }
            Chars[0] = MapText[y][x][0];
            Chars[1] = MapText[y][x][1];
            BxImage* CurFront = Front.Access(Chars);
            if(CurFront && CurFront->IsExist())
            {
                const int PosX = 32 * x + 32 - CurFront->Width();
                const int PosY = 32 * y + 32 - CurFront->Height();
                draw.Area(PosX, PosY, FORM(CurFront)); // 전경이미지
            }
            else draw.Text(Font, Chars, XYWH(32 * x + 2, 32 * y, 32, 32), textalign_center_middle); // 전경폰트
        }
    }

private:
    BxVarVector<BxImage, 256> Back;
    BxVarMap<BxImage> Front;
    id_font Font;
    int MapColorF[20][30];
    int MapColorB[20][30];
    char MapText[20][30][2];
    int LastPosX, LastPosY;
    int LastColorF, LastColorB;

public:
    /*!
        \brief 기본생성자
    */
    BxConsole(string font, int pt)
    {
        BxCore::File::SearchFiles("console/back", SearchedBack, this, nullptr);
        BxCore::File::SearchFiles("console/front", SearchedFront, this, nullptr);

		if(!BxCore::Font::IsExistNick(font))
			BxCore::Font::NickOpen(font, font);
        Font = BxCore::Font::Open(font, pt);

        for(int y = 0; y < 20; ++y)
        for(int x = 0; x < 30; ++x)
        {
            MapColorF[y][x] = 0;
            MapColorB[y][x] = 15;
            MapText[y][x][0] = '\0';
            MapText[y][x][1] = '\0';
        }

        LastPosX = 0;
        LastPosY = 0;
        LastColorF = 0;
        LastColorB = 15;
    }

    /*!
        \brief 소멸자
    */
    ~BxConsole()
    {
        BxCore::Font::Close(Font);
    }

private:
    global_func void SearchedBack(bool isdirname, string _tmp_ name, void* data1, void* data2)
    {
        if(isdirname) return;
        BxConsole* This = (BxConsole*) data1;
        const int Skip = BxUtilGlobal::StrLen("console/back/");
        const int Number = BxUtilGlobal::AtoI(name + Skip);
        This->Back[Number].Load(name, BxImage::PNG);
    }
    global_func void SearchedFront(bool isdirname, string _tmp_ name, void* data1, void* data2)
    {
        if(isdirname) return;
        BxConsole* This = (BxConsole*) data1;
        const int Skip = BxUtilGlobal::StrLen("console/front/");
        char Chars[3] = {'\0', '\0', '\0'};
        const int CharLen = BxUtilGlobal::Min(2, BxUtilGlobal::StrLen(name) - Skip - 4);
        BxUtilGlobal::StrCpy(Chars, name + Skip, CharLen);
        This->Front(Chars).Load(name, BxImage::PNG);
    }
};
