#include "stdint.h"
#include "stdio.h"
#include "sys/mman.h"

#include "shimmer.h"


int initShimmer(int fid){
	ShimmerContext* cxt = mmap((void*)0xCABBA9E000, 4096, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, fid, 0);
	if(cxt == NULL) return 0;
	
	cxt->magic          = 'SHIM';
	cxt->safety.lock	= 0;
	
	printf("CONTEXT: %p %i\n", cxt, cxt->magic);
	return 1;	
}


ShimmerContext* getContext(){
	return (void*)0xCABBA9E000;
}
