#ifndef __SHIMMER_HEADER__
#define __SHIMMER_HEADER__


#include "stdint.h"

typedef struct{
	uint8_t* data;
	uint64_t size, fill, start;
}ShimmerCircleBuf;

typedef struct{
	uint8_t* dataA;
	uint8_t* dataB;
	uint64_t size;
	uint8_t  mode;
}ShimmerDoubleBuf;

typedef struct{
	uint64_t lock;
}ShimmerLock;

typedef struct{
	uint32_t magic;
	ShimmerCircleBuf* cbufs;
	ShimmerDoubleBuf* dbufs;
	ShimmerLock     * locks;
	
	int cct, ccap, act, acap, lct, lcap;
	ShimmerLock safety;
	
	
}ShimmerContext;


int				initShimmer	(int);
ShimmerContext* getContext	();




#endif
