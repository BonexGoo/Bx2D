#pragma once
#include <BxMemory.hpp>
#include <BxPool.hpp>
#include <BxUtil.hpp>

#ifdef END
#undef END
#endif
#ifdef LAST
#undef LAST
#endif
#ifdef FINAL
#undef FINAL
#endif
#define END   ((int) 0x60000000)
#define LAST  ((int) 0x60000001)
#define FINAL ((int) 0x7FFFFFFF)

//! \brief vector식 가변배열 제공(BxVar와 동일인터페이스)
template<typename TYPE, int UNITSIZE>
class BxVarVector
{
	class VarTable;
	typedef BxPool<VarTable> TablePool;
	typedef BxPool<TYPE> TypePool;

public:
	enum Parameter {AutoCreate = 0, NoNeed = 0};
	enum Type {LengthOfRange, LengthOfCount};

	// 데이터할당
	inline TYPE& operator[](int index)
	{
		CalcIndex(index);
		ValidIndex(index);
		if(!Unit[index])
		{
			Unit[index] = TypePool::MakeClass();
			if(IsTypePointer<TYPE>())
				BxCore::Util::MemSet(Unit[index], 0, sizeof(TYPE));
			if(Size < index + 1) Size = index + 1;
			++Count;
			BytesUpdated = false;
		}
		return *Unit[index];
	}

	// 데이터접근
	inline TYPE* Access(int index) const
	{
		CalcIndex(index);
		if(Size <= index) return nullptr;
		return Unit[index];
	}

	// 데이터길이(ea)
	inline const int Length(Type type = LengthOfRange) const
	{
		if(type == LengthOfRange)
			return Size;
		return Count;
	}

	// 전체초기화
	void Reset(const bool doRemove = true)
	{
		for(int i = 0; i < Size; ++i)
		{
			if(doRemove) TypePool::FreeClass(Unit[i]);
			Unit[i] = nullptr;
		}
		Size = 0;
		Count = 0;
		BytesUpdated = false;
	}

	// 리스트삽입
	inline TYPE& Insert(int index, TYPE* newData = (TYPE*) AutoCreate)
	{
		CalcIndex(index);
		if(Size <= index)
			ValidIndex(index);
		else
		{
			ValidIndex(Size);
			BxCore::Util::MemCpy(&Unit[index + 1], &Unit[index], sizeof(TYPE*) * (Size - index));
		}
		Unit[index] = (newData)? newData : TypePool::MakeClass();
		if(!newData && IsTypePointer<TYPE>())
			BxCore::Util::MemSet(Unit[index], 0, sizeof(TYPE));
		if(Size < index + 1)
			Size = index + 1;
		else ++Size;
		++Count;
		BytesUpdated = false;
		return *Unit[index];
	}

	// 리스트삭제
	void Delete(int index, TYPE** oldData = (TYPE**) NoNeed)
	{
		CalcIndex(index);
		if(Size <= index) return;
		if(Unit[index]) --Count;
		if(oldData) *oldData = Unit[index];
		else TypePool::FreeClass(Unit[index]);
		if(index < --Size)
		{
			BxCore::Util::MemCpy(&Unit[index], &Unit[index + 1], sizeof(TYPE*) * (Size - index));
			Unit[index + (Size - index)] = nullptr;
		}
		else Unit[index] = nullptr;
		BytesUpdated = false;
	}

	// 데이터수정
	bool ModifyData(int index, TYPE* newData = (TYPE*) AutoCreate, TYPE** oldData = (TYPE**) NoNeed)
	{
		CalcIndex(index);
		if(Size <= index) return false;
		if(!Unit[index]) ++Count;
		if(oldData) *oldData = Unit[index];
		else TypePool::FreeClass(Unit[index]);
		Unit[index] = (newData)? newData : TypePool::MakeClass();
		if(!newData && IsTypePointer<TYPE>())
			BxCore::Util::MemSet(Unit[index], 0, sizeof(TYPE));
		BytesUpdated = false;
		return true;
	}

	// 데이터지움
	bool RemoveData(int index, TYPE** oldData = (TYPE**) NoNeed)
	{
		CalcIndex(index);
		if(Size <= index) return false;
		if(Unit[index]) --Count;
		if(oldData) *oldData = Unit[index];
		else TypePool::FreeClass(Unit[index]);
		Unit[index] = nullptr;
		BytesUpdated = false;
		return true;
	}

	// 데이터교체
	inline void SwapData(int index1, int index2)
	{
		TYPE* Data1 = Access(index1);
		TYPE* Data2 = Access(index2);
		TYPE* NoDelete = nullptr;
		if(!Data1 && !Data2) return;
		if(Data1) ModifyData(index2, Data1, &NoDelete);
		else RemoveData(index2, &NoDelete);
		if(Data2) ModifyData(index1, Data2, &NoDelete);
		else RemoveData(index1, &NoDelete);
	}

	// 데이터직렬화
	const byte* GetBytes()
	{
		if(!BytesUpdated)
		{
			ReleaseBytes();
			if(0 < Size)
			{
				BytesData = BxNew_Array(TYPE, Size);
				for(int i = 0; i < Size; ++i)
				{
					TYPE* Data = Access(i);
					if(Data) BxCore::Util::MemCpy(&BytesData[i], Data, sizeof(TYPE));
					else BxCore::Util::MemSet(&BytesData[i], 0, sizeof(TYPE));
				}
			}
			BytesUpdated = true;
		}
		return (const byte*) BytesData;
	}

	// 데이터직렬화 해제
	inline void ReleaseBytes()
	{
		BxDelete_Array(BytesData);
		BytesData = nullptr;
		BytesUpdated = false;
	}

	// 데이터복제
	BxVarVector& operator=(BxVarVector& RHS)
	{
		Reset();
		for(int i = 0; i < RHS.Length(); ++i)
		{
			TYPE* Data = RHS.Access(i);
			if(Data) operator[](i) = *Data;
		}
		Size = RHS.Length();
		return *this;
	}

	// 생성자
	BxVarVector() : BytesUpdated(false), BytesData(nullptr)
	{
		UnitSize = UNITSIZE;
		Unit = (TYPE**) TablePool::MakeClass();
		Size = 0;
		Count = 0;
	}
	// 생성자
	BxVarVector(const BxVarVector& RHS) : BytesUpdated(false), BytesData(nullptr)
	{
		UnitSize = UNITSIZE;
		Unit = (TYPE**) TablePool::MakeClass();
		Size = 0;
		Count = 0;
		operator=(RHS);
	}
	// 소멸자
	virtual ~BxVarVector()
	{
		ReleaseBytes();
		Reset();
		if(UnitSize == UNITSIZE)
			TablePool::FreeClass((VarTable*) Unit);
		else BxFree(Unit);
	}

private:
	class VarTable
	{
		public: TYPE* Ch[UNITSIZE];
		public: VarTable()
		{BxCore::Util::MemSet(Ch, 0x00, sizeof(TYPE*) * UNITSIZE);}
	};

	bool BytesUpdated;
	TYPE* BytesData;
	int UnitSize;
	TYPE** Unit;
	int Size;
	int Count;

	inline void CalcIndex(int& index) const
	{
		if((index & 0xE0000000) == 0x60000000)
		{
			if(index == FINAL) index = (0 < Size)? Size - 1 : 0;
			else index = index - END + Size - 1;
		}
		BxASSERT("BxVarVector", 0 <= index);
	}
	void ValidIndex(const int index)
	{
		if(index < UnitSize) return;
		const int NewUnitSize = BxUtilGlobal::Max(UnitSize + UNITSIZE, (index + 1 + UNITSIZE - 1) / UNITSIZE * UNITSIZE);
		TYPE** NewUnit = (TYPE**) BxAlloc(sizeof(TYPE*) * NewUnitSize);
		BxCore::Util::MemCpy(NewUnit, Unit, sizeof(TYPE*) * UnitSize);
		BxCore::Util::MemSet(NewUnit + UnitSize, 0x00, sizeof(TYPE*) * (NewUnitSize - UnitSize));
		if(UnitSize == UNITSIZE)
			TablePool::FreeClass((VarTable*) Unit);
		else BxFree(Unit);
		UnitSize = NewUnitSize;
		Unit = NewUnit;
	}
};
