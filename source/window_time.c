#include <windows.h>
#include<time.h>
/*
 * gStarted���Ƿ��ʼ����
 * gFreq   ��cpuƵ�ʲ���
 * gStartUs����ʼ����ʱ��ĳ�ʼֵ
 */
static int gStarted=0;
static LARGE_INTEGER gFreq;
static LARGE_INTEGER gStartUs;
static time_t gStartTime;

static void internalTimeInit()
{
	if (gStarted == 0) 
	{
		QueryPerformanceFrequency(&gFreq);
		QueryPerformanceCounter(&gStartUs);
		gStarted = 1;
		time(&gStartTime);
	}
}

static double internalGetUs()
{
	internalTimeInit();

	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	double dt = (now.QuadPart - gStartUs.QuadPart) / (double)gFreq.QuadPart;
	return dt * 1000000;
}

inline void internalGetTimeString(char* s) {
	int64_t current= (int64_t)internalGetUs();

	time_t ts = gStartTime +(current / 1000000);
	int tus=current % 1000000;

	struct tm* t=0;
	t= localtime(&ts);
#ifdef FFL_TIME_USE_SHORT_STRING
	sprintf(s,
		"%02d-%02d%02d%02d:%03d:%03d",
		t->tm_mday,
		t->tm_hour,
		t->tm_min,
		t->tm_sec,
		tus / 1000, tus % 1000);
#else
	sprintf(s,
		"%4d%02d%02d%02d%02d%02d:%03d:%03d",
		t->tm_year + 1900,
		t->tm_mon + 1,
		t->tm_mday,
		t->tm_hour,
		t->tm_min,
		t->tm_sec,
		tus / 1000, tus % 1000);
#endif
	
}


