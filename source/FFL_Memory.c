/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Memory.c
*  Created by zhufeifei(34008081@qq.com) on 2017/7/12.
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  内存的申请，释放，大小端控制 ， 内存泄漏检测
*/

#include "FFL_Memory.h"
#include "FFL.h"

#if  CHECK_FOR_MEMORY_LEAKS

#include "thread/FFL_Mutex.h"
typedef  struct MemBlock{
	void* ptr;
	int size;

	int32_t createtime;
    int32_t id;
	struct MemBlock* next;
	//struct MemBlock* pre;
}MemBlock;
static MemBlock* g_memblock_header=0;
static FFL_mutex* g_memblock_mutex=0;
static int32_t g_memblock_id=1;

static inline void init_internal_memlock()
{
    static int first=1;
    if(first)
    {
        first=0;
        /*
         *   多线程下可能错误，先这样用着
         *   zhufeifei
         * */
        if(g_memblock_mutex==0)
        {
            g_memblock_mutex=FFL_CreateMutex();
        }
    }
}

static int gInt3Table[] = { 78 };
void internal_alloc_memblock(void* ptr,int size)
{
    MemBlock* block;
    FFL_mutex* lock=g_memblock_mutex;

    FFL_LockMutex(lock);
    block=malloc(sizeof(MemBlock));
    block->createtime=FFL_getNowMs();
    block->id=g_memblock_id++;
    block->ptr=ptr;
    block->size=size;

    block->next=g_memblock_header;
    g_memblock_header=block;
    FFL_UnlockMutex(lock);	

	for (int i = 0; i < sizeof(gInt3Table) / sizeof(gInt3Table[0]); i++) {
		if (block->id == gInt3Table[i]) {
			int j = 0;
			j++;
		}
	}
}

/*
 *    返回是否成功 1 ,0
 * */
int internal_free_memblock(void* ptr,int* size)
{
    int ret=0;
    MemBlock* block;
    MemBlock* block_pre=0;
    FFL_mutex* lock=g_memblock_mutex;
    if(g_memblock_mutex && ptr==g_memblock_mutex)
    {
        return 1;
    }

    FFL_LockMutex(lock);

    block=g_memblock_header;
    block_pre=0;
    while(block)
    {
        if(block->ptr==ptr)
        {
            if(!block_pre)
                g_memblock_header=block->next;
            else
                block_pre->next=block->next;
            if(size)
                *size=block->size;
            free(block);
            ret=1;
            break;
        }

        block_pre=block;
        block=block->next;
    }


    FFL_UnlockMutex(lock);
	return ret;
}

void *FFL_malloc(size_t size)
{
    char* prefix=0;
    char * subfix=0;
    init_internal_memlock();
	void *mem = malloc(size+4+4);
	if (!mem)
		return 0;

    prefix=(char*)mem;
    prefix[0]= 0x1b;
    prefix[1]= 0x2b;
    prefix[2]= 0x3b;
    prefix[3]= 0x4b;

    mem=prefix+4  ;

    subfix=(char*)mem + size;
    subfix[0]= 0x1e;
    subfix[1]= 0x2e;
    subfix[2]= 0x3e;
    subfix[3]= 0x4e;

	internal_alloc_memblock(mem,size);
    FFL_LOG_INFO("FFL_malloc mem =%p size=%d  t= %u",mem,size,FFL_getNowMs());
	return mem;
}

void FFL_free( void *mem)
{
    char* prefix=0;
    char * subfix=0;
    int size=0;
    init_internal_memlock();
	if (mem )
	{
		if(internal_free_memblock(mem,&size)==0)
        {
            FFL_LOG_ERROR("FFL_free not find mem =%p",mem);
        }else{
            FFL_LOG_INFO("FFL_free mem =%p ",mem);
            prefix=((char*)mem)-4;
            if(prefix[0]!= 0x1b|| prefix[1]!= 0x2b||prefix[2]!= 0x3b||prefix[3]!= 0x4b)
            {
                FFL_LOG_ERROR("FFL_free prefix breakdown %p",mem);
                FFL_ASSERT_LOG(0,"memory corruption!");
            }

            subfix=(char*)mem + size;
            if(subfix[0]!= 0x1e|| subfix[1]!= 0x2e||subfix[2]!= 0x3e||subfix[3]!= 0x4e)
            {
                FFL_LOG_ERROR("FFL_free subfix breakdown %p",mem);
                FFL_ASSERT_LOG(0,"memory corruption!");
            }

            mem=prefix;
        }
		free(mem);
	}
}
/*
 *   打印一下当前还没有释放的内存
 * */
void  FFL_malloc_memory_dump()
{
    int maxid=0;
    int count=0;
    MemBlock* block;
    FFL_mutex* lock=g_memblock_mutex;
    if(!lock)
        return;

    FFL_LockMutex(lock);
    block=g_memblock_header;
    FFL_LOG_INFO("FFL_malloc_memory_dump begin ");
    while(block)
    {
        maxid=FFL_MAX(maxid,block->id);
        FFL_LOG_INFO("FFL_malloc mem =%p size=%d  createitme= %u  id=%u",
                 block->ptr,block->size,block->createtime,block->id);
        block=block->next;
        count++;
    }
    FFL_LOG_INFO("FFL_malloc_memory_dump count=%d maxid=%u end ",count,maxid);
    FFL_UnlockMutex(lock);
}
#else
void  FFL_malloc_memory_dump(){}
#endif


int FFL_outofmemory()
{
	FFL_ASSERT(0);
	FFL_LOG_ERROR("outofmemory");
	return FFL_ERROR_SUCCESS;
}

DECLSPEC char* FFL_CALL FFL_strdup(const char *s)
{
	char *ptr = NULL;
	if (s) {
		size_t len = strlen(s) + 1;
		ptr = FFL_mallocz(len);
		if (ptr)
			memcpy(ptr, s, len);
	}
	return ptr;
}

DECLSPEC char* FFL_CALL FFL_strndup(const char *s, size_t len)
{
	char *ret = NULL, *end;

	if (!s)
		return NULL;

	end = memchr(s, 0, len);
	if (end)
		len = end - s;

	ret = FFL_mallocz(len + 1);
	if (!ret)
		return NULL;

	memcpy(ret, s, len);
	ret[len] = 0;
	return ret;
}

int FFL_isLittleEndian()
{
	static int littleEndian = -1;
	if (littleEndian == -1) {
		union {
			int32_t i;
			int8_t c;
		} check;

		check.i = 0x01;
		littleEndian = check.c;
	}
	return littleEndian;
}