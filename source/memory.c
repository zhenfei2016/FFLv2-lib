
void *FFL_malloc(size_t size){
	void *mem = malloc(size);
	if (!mem)
		return 0;
	return mem;
}

void FFL_free(void *mem){
	if (mem)
	{
		free(mem);
	}
}

/*
*   打印一下当前还没有释放的内存
*/
void  FFL_dumpMemoryLeak() {

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