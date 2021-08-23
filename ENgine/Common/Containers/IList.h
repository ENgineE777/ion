
#pragma once
template<class T>
class List
{	
	char file[256];
	int  line;

	struct ElemPtr
	{		
		ElemPtr* prevPtr;
		ElemPtr* nextPtr;		
	};

	ElemPtr* firstBlock;
	ElemPtr* lastBlock;
	int     num_elemets;		

public:	
	
	List(const char* fl, int ln)
	{
		String::Copy(file, sizeof( file ), fl);
		line = ln;
		firstBlock = 0;
		lastBlock = 0;
		num_elemets = 0;
	}

	inline T* Add()
	{			
        int size = sizeof(ElemPtr) + sizeof(T);	
		byte* ptr = (byte*)malloc(size);
		
		T* object = (T*)(ptr + sizeof(ElemPtr));
		new (object) T();									

		ElemPtr* elemPtr = (ElemPtr*)ptr;
		
		elemPtr->prevPtr = lastBlock;
		if (lastBlock) lastBlock->nextPtr = elemPtr;
		elemPtr->nextPtr = 0;

		if (firstBlock == 0) firstBlock = elemPtr;
		lastBlock = elemPtr;		

		num_elemets++;

		return object;
	}

	inline T* Insert(T* ptr)
	{
		if (!firstBlock) return Add();

		ElemPtr* prevPtr = (ElemPtr*)((byte*)ptr - sizeof(ElemPtr));

		if (prevPtr == lastBlock) return Add();

		ElemPtr* nextPtr = prevPtr->nextPtr;

		T* object = (T*)(ptr + sizeof(ElemPtr));
		new (object) T();

		ElemPtr* elemPtr = (ElemPtr*)malloc(sizeof(ElemPtr));

		prevPtr->nextPtr = elemPtr;
		elemPtr->prevPtr = prevPtr;

		elemPtr->nextPtr = nextPtr;
		nextPtr->prevPtr = elemPtr;

		return object;
	}

	inline T* SwapNextFromTail(T* ptr)
	{
		if (!firstBlock)
		{
			return 0;
		}

		ElemPtr* elemPtr = (ElemPtr*)((byte*)ptr - sizeof(ElemPtr));

		if (elemPtr == lastBlock)
		{
			return 0;
		}

		ElemPtr* nextPtr = elemPtr->nextPtr;

		if (nextPtr == lastBlock)
		{
			return (T*)((byte*)nextPtr + sizeof(ElemPtr));
		}
		
		ElemPtr* last_prev = lastBlock->prevPtr;				
		
		elemPtr->nextPtr = lastBlock;
		lastBlock->prevPtr = elemPtr;

		lastBlock->nextPtr = nextPtr;
		nextPtr->prevPtr = lastBlock;

		last_prev->nextPtr = 0;

		elemPtr = lastBlock;
		lastBlock = last_prev;	

		return (T*)((byte*)elemPtr + sizeof(ElemPtr));		
	}

	inline T* GetFirst()
	{
		if (!firstBlock) return 0;

		return (T*)((byte*)firstBlock + sizeof(ElemPtr));		
	}

	inline T* GetNext(T* ptr)
	{
		if (!ptr) return 0;

		ElemPtr* elemPtr = (ElemPtr*)((byte*)ptr - sizeof(ElemPtr));

		ElemPtr* nextPtr = elemPtr->nextPtr;

		if (!nextPtr)
		{
			return 0;
		}

		return (T*)((byte*)nextPtr + sizeof(ElemPtr));
	}

	inline T* GetPrev(T* ptr)
	{
		ElemPtr* elemPtr = (ElemPtr*)((byte*)ptr - sizeof(ElemPtr));

		ElemPtr* prevPtr = elemPtr->prevPtr;

		if (!prevPtr) return 0;

		return (T*)((byte*)prevPtr + sizeof(ElemPtr));
	}	

	inline void Delete(T* ptr)
	{	
		ptr->~T();

		ElemPtr* elemPtr = (ElemPtr*)((byte*)ptr - sizeof(ElemPtr));
				
		num_elemets--;
				
		if (elemPtr->prevPtr)
		{
			elemPtr->prevPtr->nextPtr = elemPtr->nextPtr;
			if (elemPtr->nextPtr) elemPtr->nextPtr->prevPtr = elemPtr->prevPtr;
		}
		else
		{
			firstBlock = elemPtr->nextPtr;
			if (elemPtr->nextPtr) elemPtr->nextPtr->prevPtr = 0;
		}

		if (!elemPtr->nextPtr)
		{
			lastBlock = elemPtr->prevPtr;
			if (elemPtr->prevPtr) elemPtr->prevPtr->nextPtr = 0;
		}

		free(elemPtr);		
	}

	inline int Size()
	{
		return num_elemets;
	}

	inline void Clear()
	{			
		T* ptr = GetFirst();

		while (ptr)
		{
			T* next = GetNext(ptr);
			Delete(ptr);
			ptr = next;
		}		
	}
};
