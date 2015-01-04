#pragma once
#include <BxCore.hpp>
#include <BxMemory.hpp>
#include <BxPool.hpp>

//! \brief map식 가변배열 제공
template<typename TYPE, uint ENDMARK = 0, bool USE_NEW = false>
class BxVarMap
{
	class VarTable;
	class VarMap;
	class VarInfo;
	typedef BxPool<VarTable> TablePool;
	typedef BxPool<VarMap> MapPool;
	typedef BxPool<VarInfo> InfoPool;
	typedef BxPool<TYPE> TypePool;

public:
	////////////////////////////////////////////////////////////////////////////////
	// 정수Key식 데이터할당
	TYPE& operator[](uint key)
	{
		BxVarMap* CurNode = this;
		do
		{
			CurNode = CurNode->ValidChild(key & 0xF);
			key >>= 4;
		}
		while(key);
		if(!CurNode->Data)
		{
			Info->Count++;
			if(USE_NEW) CurNode->Data = new TYPE;
			else CurNode->Data = TypePool::MakeClass();
			if(IsTypePointer<TYPE>())
				BxCore::Util::MemSet(CurNode->Data, 0, sizeof(TYPE));
		}
		return *CurNode->Data;
	}

	// 문자열Key식(CP949) 데이터할당
	TYPE& operator()(string key)
	{
		BxVarMap* CurNode = this;
		do
		{
			CurNode = CurNode->ValidChild((*key & 0xF0) >> 4);
			CurNode = CurNode->ValidChild(*key & 0x0F);
			if(*key) key++;
		}
		while(*key && *key != ENDMARK);
		Info->LastTip = key;
		if(!CurNode->Data)
		{
			Info->Count++;
			if(USE_NEW) CurNode->Data = new TYPE;
			else CurNode->Data = TypePool::MakeClass();
			if(IsTypePointer<TYPE>())
				BxCore::Util::MemSet(CurNode->Data, 0, sizeof(TYPE));
		}
		return *CurNode->Data;
	}

	// 문자열Key식(UTF16) 데이터할당
	TYPE& operator()(wstring key)
	{
		BxVarMap* CurNode = this;
		do
		{
			CurNode = CurNode->ValidChild((*key & 0xF000) >> 12);
			CurNode = CurNode->ValidChild((*key & 0x0F00) >> 8);
			CurNode = CurNode->ValidChild((*key & 0x00F0) >> 4);
			CurNode = CurNode->ValidChild(*key & 0x000F);
			if(*key) key++;
		}
		while(*key && *key != ENDMARK);
		Info->LastTip = key;
		if(!CurNode->Data)
		{
			Info->Count++;
			if(USE_NEW) CurNode->Data = new TYPE;
			else CurNode->Data = TypePool::MakeClass();
			if(IsTypePointer<TYPE>())
				BxCore::Util::MemSet(CurNode->Data, 0, sizeof(TYPE));
		}
		return *CurNode->Data;
	}

	////////////////////////////////////////////////////////////////////////////////
	// 정수Key식 데이터접근
	TYPE* Access(uint key) const
	{
		const BxVarMap* CurNode = this;
		do
		{
			if(!CurNode->Table || !(CurNode = CurNode->Table->Ch[key & 0xF])) return nullptr;
			key >>= 4;
		}
		while(key);
		return CurNode->Data;
	}

	// 문자열Key식(CP949) 데이터접근
	TYPE* Access(string key) const
    {
        const BxVarMap* CurNode = this;
        do
        {
            if(!CurNode->Table || !(CurNode = CurNode->Table->Ch[(*key & 0xF0) >> 4])) return nullptr;
            if(!CurNode->Table || !(CurNode = CurNode->Table->Ch[*key & 0x0F])) return nullptr;
            if(*key) key++;
        }
        while(*key && *key != ENDMARK);
        Info->LastTip = key;
        return CurNode->Data;
    }

	// 문자열Key식(UTF16) 데이터접근
	TYPE* Access(wstring key) const
    {
        const BxVarMap* CurNode = this;
        do
        {
            if(!CurNode->Table || !(CurNode = CurNode->Table->Ch[(*key & 0xF000) >> 12])) return nullptr;
            if(!CurNode->Table || !(CurNode = CurNode->Table->Ch[(*key & 0x0F00) >> 8])) return nullptr;
            if(!CurNode->Table || !(CurNode = CurNode->Table->Ch[(*key & 0x00F0) >> 4])) return nullptr;
            if(!CurNode->Table || !(CurNode = CurNode->Table->Ch[*key & 0x000F])) return nullptr;
            if(*key) key++;
        }
        while(*key && *key != ENDMARK);
        Info->LastTip = key;
        return CurNode->Data;
    }

	////////////////////////////////////////////////////////////////////////////////
	// 정수Key식 데이터제거
	bool Remove(uint key)
	{
		if(RemoveCore(key))
		{
			Info->Count--;
			return true;
		}
		return false;
	}

	// 문자열Key식(CP949) 데이터제거
	bool Remove(string key)
    {
        if(RemoveCore(key))
        {
            Info->Count--;
            return true;
        }
        return false;
    }

	// 문자열Key식(UTF16) 데이터제거
	bool Remove(wstring key)
    {
        if(RemoveCore(key))
        {
            Info->Count--;
            return true;
        }
        return false;
    }

	////////////////////////////////////////////////////////////////////////////////
	// 전체초기화
	void Reset()
	{
		if(Info)
		{
			Info->Count = 0;
			Info->LastTip = nullptr;
		}
		if(Table)
		{
			for(uint i = 0; i < 16; ++i)
			{
				if(!Table->Ch[i]) continue;
				if(USE_NEW) delete Table->Ch[i]->Data;
				else TypePool::FreeClass(Table->Ch[i]->Data);
				Table->Ch[i]->Data = nullptr; // For child's Reset()
				if(USE_NEW) delete Table->Ch[i];
				else MapPool::FreeClass((VarMap*) Table->Ch[i]);
			}
			if(USE_NEW) delete Table;
			else TablePool::FreeClass(Table);
			Table = nullptr;
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// 데이터수량
	const int Count() const {return Info->Count;}

	////////////////////////////////////////////////////////////////////////////////
	// operator[]와 Access등에 의해 발생된 팁정보
	const void* GetLastTip() const {return Info->LastTip;}

	////////////////////////////////////////////////////////////////////////////////
	// 생성자
	BxVarMap() : Table(nullptr)
	{
		if(USE_NEW) Info = new VarInfo();
		else Info = InfoPool::MakeClass();
	}

	////////////////////////////////////////////////////////////////////////////////
	// 소멸자
	~BxVarMap()
	{
		if(Info)
		{
			if(USE_NEW) delete Info;
			else InfoPool::FreeClass(Info);
			Info = nullptr;
		}
		Reset();
	}

protected:
	BxVarMap(int) : Table(nullptr), Data(nullptr) {}

private:
	class VarTable
	{
		public: int Count;
		public: BxVarMap* Ch[16];
		public: VarTable() : Count(0)
		{BxCore::Util::MemSet(Ch, 0x00, sizeof(BxVarMap*) * 16);}
	};
	class VarMap : public BxVarMap
	{
		public: VarMap() : BxVarMap(0) {}
		public: ~VarMap() {}
	};
	class VarInfo
	{
		public: int Count;
		public: mutable const void* LastTip;
		public: VarInfo() : Count(0), LastTip(nullptr) {}
		public: ~VarInfo() {}
	};

	VarTable* Table;
	union
	{
		VarInfo* Info;
		TYPE* Data;
	};

	inline BxVarMap* ValidChild(const int i)
	{
		if(!Table)
		{
			if(USE_NEW) Table = new VarTable();
			else Table = TablePool::MakeClass();
		}
		if(!Table->Ch[i])
		{
			if(USE_NEW) Table->Ch[i] = new BxVarMap();
			else Table->Ch[i] = MapPool::MakeClass();
			Table->Count++;
		}
		return Table->Ch[i];
	}

	inline void InvalideChild(const int i)
	{
		BxASSERT("BxVarMap<이미 삭제된 Table->Ch[i]를 삭제하려 합니다>", Table->Ch[i]);
		if(USE_NEW) delete Table->Ch[i];
		else MapPool::FreeClass((VarMap*) Table->Ch[i]);
		Table->Ch[i] = nullptr;
		if(--Table->Count == 0)
		{
			if(USE_NEW) delete Table;
			else TablePool::FreeClass(Table);
			Table = nullptr;
		}
	}

	bool RemoveCore(uint key)
	{
		bool Result = false;
		BxVarMap* CurChild = nullptr;
		if(Table && (CurChild = Table->Ch[key & 0xF]))
		{
			if(key >> 4) Result = CurChild->RemoveCore(key >> 4);
			else if(CurChild->Data)
			{
				Result = true;
				if(USE_NEW) delete CurChild->Data;
				else TypePool::FreeClass(CurChild->Data);
				CurChild->Data = nullptr;
			}
			if(!CurChild->Table) InvalideChild(key & 0xF);
		}
		return Result;
	}

	bool RemoveCore(string key)
	{
		bool Result = false;
		BxVarMap* CurChild1 = nullptr;
		BxVarMap* CurChild2 = nullptr;
		if(Table && (CurChild1 = Table->Ch[(*key & 0xF0) >> 4]) && (CurChild2 = CurChild1->Table->Ch[*key & 0x0F]))
		{
			if(*key && *(key + 1)) Result = CurChild2->RemoveCore(key + 1);
			else if(CurChild2->Data)
			{
				Result = true;
				if(USE_NEW) delete CurChild2->Data;
				else TypePool::FreeClass(CurChild2->Data);
				CurChild2->Data = nullptr;
			}
			if(!CurChild2->Table) CurChild1->InvalideChild(*key & 0x0F);
			if(!CurChild1->Table) InvalideChild((*key & 0xF0) >> 4);
		}
		return Result;
	}

	bool RemoveCore(wstring key)
	{
		bool Result = false;
		BxVarMap* CurChild1 = nullptr;
		BxVarMap* CurChild2 = nullptr;
		BxVarMap* CurChild3 = nullptr;
		BxVarMap* CurChild4 = nullptr;
		if(Table && (CurChild1 = Table->Ch[(*key & 0xF000) >> 12]) && (CurChild2 = CurChild1->Table->Ch[(*key & 0x0F00) >> 8])
			&& (CurChild3 = CurChild2->Table->Ch[(*key & 0x00F0) >> 4]) && (CurChild4 = CurChild3->Table->Ch[*key & 0x000F]))
		{
			if(*key && *(key + 1)) Result = CurChild4->RemoveCore(key + 1);
			else if(CurChild4->Data)
			{
				Result = true;
				if(USE_NEW) delete CurChild4->Data;
				else TypePool::FreeClass(CurChild4->Data);
				CurChild4->Data = nullptr;
			}
			if(!CurChild4->Table) CurChild3->InvalideChild(*key & 0x000F);
			if(!CurChild3->Table) CurChild2->InvalideChild((*key & 0x00F0) >> 4);
			if(!CurChild2->Table) CurChild1->InvalideChild((*key & 0x0F00) >> 8);
			if(!CurChild1->Table) InvalideChild((*key & 0xF000) >> 12);
		}
		return Result;
	}
};
