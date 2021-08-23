
#pragma once

#define NEW_BEGIN(ptr, type, size, file, line) \
{\
	int mem_local_ptr_size = sizeof(type);\
	byte* mem_local_ptr = (byte*)core->AllocMem(mem_local_ptr_size, size, file, line);\
	for (int mem=0;mem<size;mem++)\
	{

#define NEW_IN_P0(type) \
		new (&mem_local_ptr[mem*mem_local_ptr_size]) type();

#define NEW_IN_P1(type,param1) \
		new (&mem_local_ptr[mem*mem_local_ptr_size]) type(param1);

#define NEW_IN_P2(type,param1, param2) \
		new (&mem_local_ptr[mem*mem_local_ptr_size]) type(param1, param2);

#define NEW_IN_P3(type,param1, param2, param3) \
		new (&mem_local_ptr[mem*mem_local_ptr_size]) type(param1, param2, param3);

#define NEW_IN_P4(type,param1, param2, param3, param4) \
		new (&mem_local_ptr[mem*mem_local_ptr_size]) type(param1, param2, param3, param4);

#define NEW_IN_P5(type,param1, param2, param3, param4, param5) \
		new (&mem_local_ptr[mem*mem_local_ptr_size]) type(param1, param2, param3, param4, param5);

#define NEW_IN_P6(type,param1, param2, param3, param4, param5, param6) \
		new (&mem_local_ptr[mem*mem_local_ptr_size]) type(param1, param2, param3, param4, param5, param6);

#define NEW_IN_P7(type,param1, param2, param3, param4, param5, param6, param7) \
		new (&mem_local_ptr[mem*mem_local_ptr_size]) type(param1, param2, param3, param4, param5, param6, param7);

#define NEW_END(ptr,type) \
		}\
	ptr = (type*)mem_local_ptr;\
}

#ifdef _DEBUGLEAKS
#define NEWFL(ptr, type, size, file, line)	ptr = new type();
#else
#define NEWFL(ptr, type, size, file, line) \
NEW_BEGIN(ptr, type, size, file, line) \
NEW_IN_P0(type) \
NEW_END(ptr,type)
#endif

#define NEWFL1(ptr, type,size,param1, file, line) \
NEW_BEGIN(ptr, type,size, file, line) \
NEW_IN_P1(type,param1) \
NEW_END(ptr,type)

#define NEWFL2(ptr, type,size,param1, param2, file, line) \
NEW_BEGIN(ptr, type,size, file, line) \
NEW_IN_P2(type,param1, param2) \
NEW_END(ptr,type)

#define NEWFL3(ptr, type,size,param1, param2, param3, file, line) \
NEW_BEGIN(ptr, type,size, file, line) \
NEW_IN_P3(type,param1, param2, param3) \
NEW_END(ptr,type)

#define NEWFL4(ptr, type,size,param1, param2, param3, param4, file, line) \
NEW_BEGIN(ptr, type,size, file, line) \
NEW_IN_P4(type,param1, param2, param3, param4) \
NEW_END(ptr,type)

#define NEWFL5(ptr, type,size,param1, param2, param3, param4, param5, file, line) \
NEW_BEGIN(ptr, type,size, file, line) \
NEW_IN_P5(type,param1, param2, param3, param4, param5) \
NEW_END(ptr,type)

#define NEWFL6(ptr, type,size,param1, param2, param3, param4, param5, param6, file, line) \
NEW_BEGIN(ptr, type,size, file, line) \
NEW_IN_P6(type,param1, param2, param3, param4, param5, param6) \
NEW_END(ptr,type)

#define NEWFL7(ptr, type,size, param1, param2, param3, param4, param5, param6, param7, file, line) \
NEW_BEGIN(ptr, type,size, file, line) \
NEW_IN_P7(type,param1, param2, param3, param4, param5, param6, param7) \
NEW_END(ptr,type)



#define NEW(ptr, type, size) \
NEWFL(ptr, type, size, __FILE__, __LINE__)

#define NEW1(ptr, type, size, param1) \
NEWFL1(ptr, type, size, param1, __FILE__, __LINE__)

#define NEW2(ptr, type, size, param1, param2) \
NEWFL2(ptr, type, size, param1, param2, __FILE__, __LINE__)

#define NEW3(ptr, type, size, param1, param2, param3) \
NEWFL3(ptr, type, size, param1, param2, param3, __FILE__, __LINE__)

#define NEW4(ptr, type, size, param1, param2, param3, param4) \
NEWFL4(ptr, type, size, param1, param2, param3, param4, __FILE__, __LINE__)

#define NEW5(ptr, type, size, param1, param2, param3, param4, param5) \
NEWFL5(ptr, type, size, param1, param2, param3, param4, param5, __FILE__, __LINE__)

#define NEW6(ptr, type, size, param1, param2, param3, param4, param5, param6) \
NEWFL6(ptr, type, size, param1, param2, param3, param4, param5, param6, __FILE__, __LINE__)

#define NEW7(ptr, type, size, param1, param2, param3, param4, param5, param6, param7) \
NEWFL7(ptr, type, size, param1, param2, param3, param4, param5, param6, param7, __FILE__, __LINE__)

#ifdef _DEBUGLEAKS
#define DEL(ptr, type) delete ptr;
#else
#define DEL(ptr, type)\
{\
	if (ptr)\
	{\
		int mem_local_ptr_size = sizeof(type);\
		int size = core->GetMemBlockElen(ptr);\
		for (int mem=0;mem<size;mem++)\
		{\
			(ptr)[mem].~type();\
		}\
		core->FreeMem(ptr);\
	}\
}
#endif

#define DELD(ptr, type, destructor)\
{\
	if (ptr)\
	{\
	int mem_local_ptr_size = sizeof(type);\
	int size = core->GetMemBlockElen(ptr);\
	for (int mem=0;mem<size;mem++)\
		{\
		(ptr)[mem].~destructor();\
		}\
		core->FreeMem(ptr);\
	}\
}
//void* operator new(size_t size);
//void* operator new[](size_t size);
//inline void operator delete[](void* p);
//inline void operator delete[](void* p, size_t size);