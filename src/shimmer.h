#ifndef __SHIMMER_HEADER__
#define __SHIMMER_HEADER__

#include "stdint.h"



typedef struct{
	uint64_t xs[4];
}SHMObj;

typedef struct{
	uint32_t	magic;
	uint32_t	size, fill;
	uint32_t	next;
}ShimmerContext;

typedef struct{
	uint64_t**	buffers;
	uint64_t*	sizes;
	int			size, fill;
	char*		path;
}Shimmer;


int		initShimmer   (Shimmer*, char*, int);
int		connectShimmer(Shimmer*, char*, int);








#endif
