#include "thread/FFL_Mutex.h"

typedef  struct MemBlock {
	void* ptr;
	int size;

	int32_t createtime;
	int32_t id;
	struct MemBlock* next;
	//struct MemBlock* pre;
}MemBlock;
static MemBlock* g_memblock_header = 0;
static FFL_mutex* g_memblock_mutex = 0;
static int32_t g_memblock_id = 1;

/*
*  初始化内存锁
*/
static void internalInitMemlock();
/*
*  申请内存
*/
static void internalAllocMemBlock(void* ptr, int size);
/*
*    释放内存，返回是否成功 1 ,0
* */
static int internalFreeMemblock(void* ptr, int* size);

void *FFL_malloc(size_t size)
{
	char* prefix = 0;
	char * subfix = 0;
	internalInitMemlock();
	void *mem = malloc(size + 4 + 4);
	if (!mem)
		return 0;

	prefix = (char*)mem;
	prefix[0] = 0x1b;
	prefix[1] = 0x2b;
	prefix[2] = 0x3b;
	prefix[3] = 0x4b;

	mem = prefix + 4;

	subfix = (char*)mem + size;
	subfix[0] = 0x1e;
	subfix[1] = 0x2e;
	subfix[2] = 0x3e;
	subfix[3] = 0x4e;

	internalAllocMemBlock(mem, size);
	FFL_LOG_INFO("FFL_malloc mem =%p size=%d  t= %u", mem, size, FFL_getNowMs());
	return mem;
}

/*
*  释放的时候，会检测内存是否给破坏了
*/
void FFL_free(void *mem)
{
	char* prefix = 0;
	char * subfix = 0;
	int size = 0;
	internalInitMemlock();
	if (mem)
	{
		if (internalFreeMemblock(mem, &size) == 0)
		{
			FFL_LOG_ERROR("FFL_free not find mem =%p", mem);
		}
		else {
			FFL_LOG_INFO("FFL_free mem =%p ", mem);
			prefix = ((char*)mem) - 4;
			if (prefix[0] != 0x1b || prefix[1] != 0x2b || prefix[2] != 0x3b || prefix[3] != 0x4b)
			{
				FFL_LOG_ERROR("FFL_free prefix breakdown %p", mem);
				FFL_ASSERT_LOG(0, "memory corruption!");
			}

			subfix = (char*)mem + size;
			if (subfix[0] != 0x1e || subfix[1] != 0x2e || subfix[2] != 0x3e || subfix[3] != 0x4e)
			{
				FFL_LOG_ERROR("FFL_free subfix breakdown %p", mem);
				FFL_ASSERT_LOG(0, "memory corruption!");
			}

			mem = prefix;
		}
		free(mem);
	}
}

/*
*   打印一下当前还没有释放的内存
*/
void  FFL_dumpMemoryLeak() {
	int maxid = 0;
	int count = 0;
	MemBlock* block;
	FFL_mutex* lock = g_memblock_mutex;
	if (!lock)
		return;

	FFL_LockMutex(lock);
	block = g_memblock_header;
	FFL_LOG_INFO("FFL_malloc_memory_dump begin ");
	while (block)
	{
		maxid = FFL_MAX(maxid, block->id);
		FFL_LOG_INFO("FFL_malloc mem =%p size=%d  createitme= %u  id=%u",
			block->ptr, block->size, block->createtime, block->id);
		block = block->next;
		count++;
	}
	FFL_LOG_INFO("FFL_malloc_memory_dump count=%d maxid=%u end ", count, maxid);
	FFL_UnlockMutex(lock);
}
/*
*  打印当前未释放的内存，到文件中
*/
void  FFL_dumpMemoryLeakFile(const char* path) {

}
/*
*  参考上一次释放的内存文件，打印对应的堆栈
*/
void  FFL_checkMemoryLeak(const char* path) {

}

/*******************************************************************************************/
/*******************************************************************************************/
static int gInt3Table[] = { 78 };
/*
* 是否内存泄漏的id
*/
static int internalIsMemoryLeakId(int32_t id) {
	for (int i = 0; i < sizeof(gInt3Table) / sizeof(gInt3Table[0]); i++) {
		if (id== gInt3Table[i]) {
			assert(0);
			break;
		}
	}
}

/*
 *打印调用栈
 */
static void internalPrintCallStack() {
	assert("internalPrintCallStack");
}


static void internalInitMemlock()
{
	static int first = 1;
	if (first)
	{
		first = 0;
		/*
		*   多线程下可能错误，先这样用着
		*   zhufeifei
		* */
		if (g_memblock_mutex == 0)
		{
			g_memblock_mutex = FFL_CreateMutex();
		}
	}
}


static void internalAllocMemBlock(void* ptr, int size)
{
	MemBlock* block;
	FFL_mutex* lock = g_memblock_mutex;

	FFL_LockMutex(lock);
	block = malloc(sizeof(MemBlock));
	block->createtime = FFL_getNowMs();
	block->id = g_memblock_id++;
	block->ptr = ptr;
	block->size = size;

	block->next = g_memblock_header;
	g_memblock_header = block;
	FFL_UnlockMutex(lock);

	if (internalIsMemoryLeakId(block->id)) {
		internalPrintCallStack();
	}
}

/*
*    返回是否成功 1 ,0
* */
static int internalFreeMemblock(void* ptr, int* size)
{
	int ret = 0;
	MemBlock* block;
	MemBlock* block_pre = 0;
	FFL_mutex* lock = g_memblock_mutex;
	if (g_memblock_mutex && ptr == g_memblock_mutex){
		return 1;
	}

	FFL_LockMutex(lock);

	block = g_memblock_header;
	block_pre = 0;
	while (block)
	{
		if (block->ptr == ptr)
		{
			if (!block_pre)
				g_memblock_header = block->next;
			else
				block_pre->next = block->next;
			if (size)
				*size = block->size;
			free(block);
			ret = 1;
			break;
		}

		block_pre = block;
		block = block->next;
	}


	FFL_UnlockMutex(lock);
	return ret;
}

