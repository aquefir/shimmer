#ifndef __SHIMMER_HEADER__
#define __SHIMMER_HEADER__


#include "stdint.h"


typedef struct{
	uint64_t*	buffer;
	uint32_t	size;
	uint32_t	head;
}SHMFeed;

typedef struct{
	uint64_t*	buffer;
	uint32_t	size;
}SHMBuff;

typedef struct{
	uint32_t	lock;
}SHMLock;

typedef enum{
	SHMK_NULL	= 0,
	SHMK_FEED	= 1,
	SHMK_BUFF	= 2,
	SHMK_LOCK	= 3
}SHMKind;

typedef struct{
	char		name[16];
	union{
		SHMFeed		feed;
		SHMBuff		buff;
		SHMLock		lock;
	};
	SHMKind		kind;
}SHMObj;


typedef struct{
	SHMObj*		objs;
	int			objct, objcap;
	SHMLock		lock;
	
	char*		filename;
	int			bufferIndex;
}SHMTab;



#endif
