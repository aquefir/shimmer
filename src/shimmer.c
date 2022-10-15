#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "shimmer.h"



// ftok		: file  -> key
// shmget	: key   -> block
// shmat	: block -> ptr

int roundUpExp2(uint64_t x){
	int lz = __builtin_clzl(x);
	return 1l << (63-lz);
}


int			getSharedBuffer(char* filename, int size, int index){
	key_t	key;
	
	key = ftok(filename, index);
	if(key < 0) return -1;
	
	return shmget(key, size, 0644 | IPC_CREAT);
}

uint64_t*	getMemoryBlock(char* filename, int size, int index){
	int bufferId = getSharedBuffer(filename, size, index);
	uint64_t* ret;
	
	if(bufferId < 0) return NULL;
	
	ret = shmat(bufferId, 0, index);
	if(ret == NULL) return NULL;
	
	return ret;
}

int			detatchSharedBuffer(uint64_t* buffer){
	return (shmdt(buffer) != -1);
}

int			destroySharedBuffer(char* filename, int index){
	int bufferId = getSharedBuffer(filename, 0, index);
	if(bufferId == -1) return 0;
	
	return (shmctl(bufferId, IPC_RMID, 0) != -1);
}


SHMTab*		initShimmerTab(char* filename, int size){
	int flen = strlen(filename);
	if((flen+sizeof(SHMTab)+8192) > size){
		printf("Buffer size too small\n");
		return NULL;
	}

	uint64_t* buffer = getMemoryBlock(filename, size, 0);
	if(buffer       == NULL){
		printf("Shared memory buffer could not be created\n");
		return NULL;
	}
	uint8_t*  bytes  = (uint8_t*)buffer;
	char*     fname  = (char*)&bytes[sizeof(SHMTab)];
	strcpy(fname, filename);
	
	SHMTab* ret    = (SHMTab*)buffer;
	ret->magic     = 0xC4133378;
	ret->filename  = fname;
	ret->size      = size;
	ret->bufferTop = 0;
	
	return ret;
}

SHMTab*		connectShimmerTab(char* filename, int size){
	uint64_t* buffer = getMemoryBlock(filename, size, 0);
	if(buffer       == NULL){
		printf("Shared memory buffer could not be located\n");
		return NULL;
	}
	SHMTab*   ret    = (SHMTab*)buffer;
	if(ret->magic   != 0xC4133378){
		printf("Shared memory buffer is not properly initialized\n");
		return NULL;
	}
	return ret;
}


