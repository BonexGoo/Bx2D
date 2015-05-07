#pragma once
#include <BxCore.hpp>

//! \brief 큐 제공
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
	// 큐에 1개 데이터추가
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

	// 큐에서 1개 데이터배출
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
