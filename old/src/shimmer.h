#ifndef __SHIMMER_HEADER__
#define __SHIMMER_HEADER__


#include "stdint.h"


typedef struct{
	uint32_t	size;
	uint32_t	head;
}SHMFeed;

typedef struct{
	uint32_t	size;
}SHMBuff;

typedef struct{
	uint32_t	pagect;
	uint32_t	pagesize;
	uint32_t	pageread;
	uint32_t	pagehead;	
}SHMPage;


typedef struct{
	uint32_t	lock;
}SHMLock;

typedef enum{
	SHMK_NULL	= 0x00,
	SHMK_FREE	= 0x01,
	
	SHMK_FEED	= 0x10,
	SHMK_BUFF	= 0x11,
	SHMK_PAGE	= 0x12,
	SHMK_LOCK	= 0x13
}SHMKind;

typedef struct{
	char		name[16];
	uint8_t*	p_buffer;
	uint8_t*	c_buffer;
	uint32_t	bufferIx;
	union{
		SHMFeed		feed;
		SHMBuff		buff;
		SHMPage		page;
		SHMLock		lock;
	};
	SHMKind		kind;
}SHMObj;

typedef struct{
	SHMObj*		p_objs;
	SHMObj*		c_objs;
	int			objct, opjcap;
	int			nextIx;
}SHMObjTable;

typedef struct{
	uint32_t	magic;
	SHMObjTable	table;
	SHMLock		lock;
	
	uint32_t	bufferTop, bufferCap, size, bufferOffset;
}SHMTab;

typedef struct{
	SHMTab*		shtab;
	
	void**		pointers;
	int64_t*	sizes;
	int64_t		size, fill;
}SHM;


SHM			initShimmer			(char*, uint32_t);
SHM 		connectShimmer		(char*, uint32_t);
SHMTab*		initShimmerTab		(char*, uint32_t);
SHMTab*		connectShimmerTab	(char*, uint32_t);
char*		getFilename			(SHMTab*);

int32_t		makeSHMFeed			(SHM*, char*, uint32_t);
int32_t		makeSHMBuff			(SHM*, char*, uint32_t);
int32_t		makeSHMPage			(SHM*, char*, uint32_t, uint32_t);
int32_t		makeSHMLock			(SHM*, char*, uint32_t);

void		printSHMTab			(SHM*);


/*
	TODO:
	x shared table:
		x pars : file, bufct
		x allocate >= space to SHMObj[bufct], round up to nearest 4k page
	x index(SHMTab*, int) -> SHMObj*
	_ local pointer table for getting pointers to pages
	x should be able to get a table of shmobjs and print it to the terminal
	* shmlock : timing mechanism for efficient sleeping?
	* shmbuff : raw byte buffer
	* shmpage : array of raw byte buffers, rd/wt indices, lock on collision
	* shmfeed : insert into queue, lock when full
	* remapPtr(void*, SHMPtrTab) -> void*

*/

#endif
