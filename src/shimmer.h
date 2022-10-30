#ifndef __SHIMMER_HEADER__
#define __SHIMMER_HEADER__

#include "stdint.h"


typedef enum{
	SH_NULL = 0,
	SH_LOCK = 1,
	SH_BUFF = 2
}SHMKind;

typedef struct{
	uint32_t state;
}SHMLock;

typedef struct{
	uint64_t size;
}SHMBuff;

typedef struct{
	union{
		SHMLock	lock;
		SHMBuff buff;
	};
	int32_t		shmix;
	SHMKind		kind;
}SHMObj;

typedef struct{
	uint32_t	magic;
	uint32_t	size, fill;
}ShimmerContext;

typedef struct{
	uint64_t**	buffers;
	uint64_t*	sizes;
	int			size, fill;
	char*		path;
}Shimmer;


int			initShimmer   	(Shimmer*, char*, int);
int			connectShimmer	(Shimmer*, char*, int);
SHMObj*		SHM_index		(Shimmer*, int);

SHMBuff*	newSHMBuff		(Shimmer*, int);
SHMLock*	newSHMLock		(Shimmer*, int);

int			SHM_openLock	(SHMLock*);
int			SHM_setLock		(SHMLock*, int);
int			SHM_checkLock	(SHMLock*);
uint8_t*	SHM_bufferPtr	(SHMBuff*, Shimmer*);


#endif
