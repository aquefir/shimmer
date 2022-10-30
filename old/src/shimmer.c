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
	uint8_t*  bytes   = (uint8_t*)buffer;
	char*     fname   = (char*)&bytes[sizeof(SHMTab)];
	strcpy(fname, filename);
	
	SHMTab* ret       = (SHMTab*)buffer;
	ret->magic        = 0xC4133378;
	ret->size         =  size;
	ret->bufferCap    = (size - (flen + 8 + sizeof(SHMTab))) / sizeof(SHMObj);
	ret->bufferTop    = 0;
	ret->bufferOffset = (flen + 8 + sizeof(SHMTab)) & 0xfffffffffffffff8;
	
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

SHMObj* SHM_index(SHMTab* shtab, int ix){
	if(ix <                0){
		//printf("0\n");
		return NULL;
	}
	if(ix > shtab->bufferCap){
		//printf("of\n");
		return NULL;
	} // FIXME
	
	SHMObj* objs = (SHMObj*)&(((uint64_t*)shtab)[shtab->bufferOffset/8]);
	//printf("SHTAB = %p, OBJS = %p\n", shtab, objs);
	return &objs[ix];
}



SHMObj* prepSHMObj (SHMTab* shtab, char* name, uint32_t size){
	if(shtab->bufferTop+1 >= shtab->bufferCap){
		// For now, just assume we don't overflow the SHMObj table
		printf("ope\n");
	}
	shtab->bufferTop++;
	SHMObj* ret = SHM_index(shtab, shtab->bufferTop-1);
	for(int i = 0; i < 15; i++){
		ret->name[i] = name[i];
		if(name[i] == 0) break;
	}
	ret->name[15] = 0;
	return ret;
}

void printSHMTab(SHM* shm){
	SHMTab* shtab = shm->shtab;
	printf("MAGIC : %08x\n"   , shtab->magic);
	printf("LOCK  : %08x\n"   , shtab->lock.lock);
	printf("BUFFER: [%i/%i]\n", shtab->bufferTop, shtab->bufferCap);
	
	for(int i = 0; i < shtab->bufferTop; i++){
		SHMObj* obj = SHM_index(shtab, i);
		if(obj != NULL){
			printf("OBJ %i : ", i);
			switch(obj->kind){
				case SHMK_NULL	: printf("NULL\n"); break;
				case SHMK_FREE	: printf("FREE\n"); break;
	
				case SHMK_FEED	: printf("FEED | \"%s\"\n", obj->name); break;
				case SHMK_BUFF	: printf("BUFF | \"%s\"\n", obj->name); break;
				case SHMK_PAGE	: printf("PAGE | \"%s\"\n", obj->name); break;
				case SHMK_LOCK	: printf("LOCK | \"%s\"\n", obj->name); break;
			}
		}
	}
}


SHM initShimmer(char* filename, uint32_t buffsz){
	SHM ret;
	int scale    = (buffsz & 1023)? (buffsz * 4) & ~1023l : buffsz * 2;
	ret.shtab    = initShimmerTab(filename, buffsz);
	ret.pointers = malloc(sizeof(void*  ) * buffsz);
	ret.sizes    = malloc(sizeof(int64_t) * buffsz);
	ret.size     = buffsz;
	ret.fill     = 0;
	return ret;
}

SHM connectShimmer(char* filename, uint32_t buffsz){
	SHM ret;
	int scale    = (buffsz & 1023)? (buffsz * 4) & ~1023l : buffsz * 2;
	ret.shtab    = connectShimmerTab(filename, buffsz);
	ret.pointers = malloc(sizeof(void*  ) * buffsz);
	ret.sizes    = malloc(sizeof(int64_t) * buffsz);
	ret.size     = buffsz;
	ret.fill     = 0;
	return ret;
}


/*
	Allocating new pages is not entirely thread safe. It's thread safe across the shared memory
	barrer; no need to synchronize between the program and its viewer. Inside the program, or
	inside the viewer, it's less safe, you want to make sure no two threads within a program
	try to allocate a page at the same time. That said, this really shouldn't be that hard, you
	probably shouldn't be allocating a million pages anyway.
	
	This applies to:
	makeSHMFeed
	makeSHMBuff
	makeSHMPage
	makeSHMLock
	
	It also technically applies to some of the other initialization functions, but honestly I'm
	not designing this around having multiple shimmer instances in a single program.
*/
int32_t	makeSHMFeed(SHM* shm, char* name, uint32_t size){
	SHMTab*   shtab  = shm->shtab;
	uint64_t* buffer = getMemoryBlock(name, size, shtab->bufferTop);
	shm->pointers[shtab->bufferTop] = buffer;
	shtab->bufferTop++;
	SHMObj* obj      = prepSHMObj(shtab, name, size);
	obj->kind        = SHMK_FEED;
	
	return shtab->bufferTop-1;
}


int32_t	makeSHMBuff(SHM* shm, char* name, uint32_t size){
	SHMTab*   shtab  = shm->shtab;
	uint64_t* buffer = getMemoryBlock(name, size, shtab->bufferTop);
	shm->pointers[shtab->bufferTop] = buffer;
	shtab->bufferTop++;
	SHMObj* obj      = prepSHMObj(shtab, name, size);
	obj->kind        = SHMK_BUFF;

	return shtab->bufferTop-1;
}


int32_t	makeSHMPage(SHM* shm, char* name, uint32_t size, uint32_t pagect){
	SHMTab*   shtab  = shm->shtab;
	uint64_t* buffer = getMemoryBlock(name, size, shtab->bufferTop);
	shm->pointers[shtab->bufferTop] = buffer;
	shtab->bufferTop++;
	uint32_t bufferSize = size * pagect;
	SHMObj* obj      = prepSHMObj(shtab, name, bufferSize);
	obj->kind        = SHMK_PAGE;
	
	return shtab->bufferTop-1;
}


int32_t	makeSHMLock(SHM* shm, char* name, uint32_t init){
	SHMTab*   shtab  = shm->shtab;
	shtab->bufferTop++;
	SHMObj* obj      = prepSHMObj(shtab, name, 0);
	obj->kind        = SHMK_LOCK;
	obj->lock.lock   = init;
	return shtab->bufferTop-1;
}



uint32_t SHM_isLocked(SHMLock* lock){
	return (lock->lock) != 0;
}





