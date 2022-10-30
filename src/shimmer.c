#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "shimmer.h"

int roundUpExp2(uint64_t x){
	int lz = __builtin_clzl(x);
	return 1l << (63-lz);
}



int initShimmer   (Shimmer* ret, char* file, int size){
	int bufsize  = (size * sizeof(SHMObj)) + sizeof(ShimmerContext) + strlen(file) + 8;
	bufsize      = (bufsize < 4096)? 4096 : roundUpExp2(bufsize);
	
	size         = (bufsize - (8 + sizeof(ShimmerContext))) / sizeof(SHMObj);
	ret->buffers = malloc(sizeof(uint64_t*) * size);
	ret->sizes   = malloc(sizeof(uint64_t ) * size);
	ret->size    = size;
	ret->fill    = 1;
	ret->path    = malloc(sizeof(char) * strlen(file)+1);
	strcpy(ret->path, file);
	
	key_t	key;
	key = ftok(file, 0);
	if(key < 0) return 0;
	int shmid = shmget(key, bufsize, 0644 | IPC_CREAT);
	if(shmid < 0) return 0;
	ret->buffers[0] = shmat(shmid, 0, 0);
	ret->sizes  [0] = bufsize;
	
	ShimmerContext* cxt = (ShimmerContext*)ret->buffers[0];
	cxt->magic = 0xC4133378;
	
	
	return 1;
}


int connectShimmer(Shimmer* ret, char* file, int size){
	int bufsize  = (size * sizeof(SHMObj)) + sizeof(ShimmerContext) + strlen(file) + 8;
	bufsize      = (bufsize < 4096)? 4096 : roundUpExp2(bufsize);

	key_t	key;
	key = ftok(file, 0);
	if(key < 0) return 0;
	int shmid = shmget(key, bufsize, 0644);
	if(shmid < 0) return 0;
	
	size         = (bufsize - (8 + sizeof(ShimmerContext))) / sizeof(SHMObj);
	ret->buffers = malloc(sizeof(uint64_t*) * size);
	ret->sizes   = malloc(sizeof(uint64_t ) * size);
	ret->size    = size;
	ret->fill    = 1;
	ret->path    = malloc(sizeof(char) * strlen(file)+1);
	strcpy(ret->path, file);
	
	ret->buffers[0] = shmat(shmid, 0, 0);
	ret->sizes  [0] = bufsize;
	
	ShimmerContext* cxt = (ShimmerContext*)ret->buffers[0];
	if(cxt->magic != 0xC4133378) return 0;
	

	return 1;
}
