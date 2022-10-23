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

int	getSharedBuffer(char* filename, int size, int index){
	key_t	key;
	key = ftok(filename, index);
	if(key < 0) return -1;
	return shmget(key, size, 0644 | IPC_CREAT);
}

uint64_t* getMemoryBlock(char* filename, int size, int index){
	int bufferId = getSharedBuffer(filename, size, index);
	uint64_t* ret;
	if(bufferId < 0) return NULL;
	ret = shmat(bufferId, 0, index);
	if(ret == NULL) return NULL;
	return ret;
}

int	detatchSharedBuffer(uint64_t* buffer){
	return (shmdt(buffer) != -1);
}

int	destroySharedBuffer(char* filename, int index){
	int bufferId = getSharedBuffer(filename, 0, index);
	if(bufferId == -1) return 0;
	return (shmctl(bufferId, IPC_RMID, 0) != -1);
}

char* getFilename(SHMTab* shtab){
	char* buffer = (char*)shtab;
	return (char*)&buffer[sizeof(SHMTab)];
}

SHMTab*	initShimmerTab(char* filename, uint32_t buffsz){
	int flen = strlen(filename);
	
	int size = flen + 8 + sizeof(SHMTab) + (sizeof(SHMObj) * buffsz);
	size     = ((size / 4096) + ((size % 4096) != 0)) * 4096;
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
	ret->size      =  size;
	ret->bufferCap = (size - (flen + 8 + sizeof(SHMTab))) / sizeof(SHMObj);
	ret->bufferTop = 0;
	
	return ret;
}

SHMTab*	connectShimmerTab(char* filename, uint32_t size){
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



SHMObj* prepSHMObj (SHMTab* shtab, char* name, uint32_t size){
	/*
		Allocate SHMObj to table, extending table if necessary
		Set name
		If size != 0, allocate a buffer, set child buffer pointer, bufferIx
		Return SHMObj pointer
	*/
	return NULL;
}



int32_t	makeSHMFeed(SHMTab* shtab, char* name, uint32_t size){
	SHMObj* obj = prepSHMObj(shtab, name, size);
	
	return 0;
}


int32_t	makeSHMBuff(SHMTab* shtab, char* name, uint32_t size){
	SHMObj* obj = prepSHMObj(shtab, name, size);

	return 0;
}


int32_t	makeSHMPage(SHMTab* shtab, char* name, uint32_t size, uint32_t pagect){
	uint32_t bufferSize = size * pagect;
	SHMObj* obj = prepSHMObj(shtab, name, bufferSize);
	
	return 0;
}


int32_t	makeSHMLock(SHMTab* shtab, char* name, uint32_t init){
	SHMObj* obj = prepSHMObj(shtab, name, 0);

	return 0;
}

void printSHMTab(SHMTab* shtab){
	printf("MAGIC : %08x\n"   , shtab->magic);
	printf("LOCK  : %08x\n"   , shtab->lock.lock);
	printf("BUFFER: [%i/%i]\n", shtab->bufferTop, shtab->size);
	
	char   * shpath = &((char   *)shtab)[sizeof(SHMTab)];
	uint8_t* buffer = &((uint8_t*)shtab)[sizeof(SHMTab) + strlen(shpath)];
	
	SHMObjTable* tab = &shtab->table;
	while(tab != NULL){
		printf("  TAB\n");
		if(tab->nextIx)
			tab = (SHMObjTable*)getMemoryBlock(shpath, 0, tab->nextIx);
		else
			tab = NULL;
	}
}







