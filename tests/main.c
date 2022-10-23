#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "../src/shimmer.h"


int main(){
	printf("Test program\n");
	
	SHMTab* shtab =	initShimmerTab("shmem", 128);
	printf("%p %s\n", shtab, getFilename(shtab));
	int buff = makeSHMBuff(shtab, "buff", 8192);
	int feed = makeSHMFeed(shtab, "feed", 8192);
	
	printSHMTab(shtab);
	
	char text[1024];
	fgets(text, 1024, stdin);
}
