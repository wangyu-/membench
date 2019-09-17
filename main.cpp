#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "CycleTimer.h"

#include <immintrin.h>
#include <cstdint>
#include <assert.h>

#include "FastMemcpy_Avx.h"
/* ... */

void* memcpy2(void* dest, const void* src, size_t count) {
	char* dst8 = (char*)dest;
	char* src8 = (char*)src;
	--src8;
	--dst8;

	while (count--) {
		*++dst8 = *++src8;
	}
	return dest;
}

typedef unsigned long long u64_t;   //this works on most platform,avoid using the PRId64
typedef long long i64_t;

typedef unsigned int u32_t;
typedef int i32_t;

#define mytype float

const unsigned long total=1*1024*1024*1024;
//const unsigned long total=200l*1024*1024;
//const unsigned long total= 400 * 1000 * 1000;
mytype *src;
mytype *src2;
mytype *dst;
mytype scale=2.0f;
const int worker_number=16;

u64_t get_current_time()
{
	timespec tmp_time;
	clock_gettime(CLOCK_MONOTONIC, &tmp_time);
	return tmp_time.tv_sec*1000+tmp_time.tv_nsec/(1000*1000l);
}


void *worker(void *par)
{
	int tid=(int)(long long)par;
	u64_t span=total/worker_number;
	u64_t begin=tid*span;
	u64_t end=begin+span;
	u64_t start_time=get_current_time();
	printf("begin tid=%d,working on from %llu to %llu, time=%llu\n",tid,begin,end,start_time);
	//memcpy(dst+tid*span,src+tid*span,span);
	//memcpy2(dst,src,span);

/*	
	mytype *a=dst+begin;
	mytype *b=src+begin;
	size_t count=end-begin;
	while(count--)
	{
		*a++=*b++;
	}*/	
	for(int num=0;num<1;num++)
	{	/*	
		for(u64_t i=begin;i<end;i++)
		{
			//dst[i]=src[i]*scale+src2[i];
			//dst[i]=src[i];
		}*/
		memcpy2(src,dst,span*sizeof(mytype));
	}
/*
	memcpy2(dst,src,span);
	memcpy2(dst,src,span);
	memcpy2(dst,src,span);
	memcpy2(dst,src,span);
	memcpy2(dst,src,span);
	*/
	u64_t finish_time=get_current_time();
	printf("end tid=%d,time used=%lld\n",tid,finish_time-start_time);	
}

int main()
{
	src =new mytype[total+1024];
	src2 =new mytype[total+1024];
	dst= new mytype[total+1024];

	printf("%d %d %d\n", ((u64_t)src)%32,((u64_t)src2)%32,((u64_t)dst)%32);
	for(int i=0;i<total;i++)
	{
		src[i]=src2[i]=i;
		dst[i]=0;
	}
	
	/*u64_t begin_time=get_current_time();
	memcpy(dst,src,total*sizeof(float));
	u64_t end_time=get_current_time();
	
	printf("time used=%lld ms\n",end_time-begin_time);*/
	
	u64_t start_time=get_current_time();
        double startTime =CycleTimer::currentSeconds();


	pthread_t thread[worker_number];

	for(int i=0;i<worker_number;i++)
	{
		pthread_create(&thread[i], 0, worker, (void *)i);
		
	}
	for(int i=0;i<worker_number;i++)
	{
		pthread_join(thread[i],0);
		
	}
//	sleep(2);
	u64_t finish_time=get_current_time();
        double endTime = CycleTimer::currentSeconds();

	printf("total time used=%lld ms , %f sec\n",finish_time-start_time,endTime-startTime);
	
	return 0;
}
