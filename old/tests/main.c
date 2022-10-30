#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "../src/shimmer.h"


int main(){
	printf("Test program\n");
	
	SHM shm = initShimmer("shmem", 128);
	printf("%p %s\n", shm.shtab, getFilename(shm.shtab));
	int buff = makeSHMBuff(&shm, "buff", 8192);
	int feed = makeSHMFeed(&shm, "feed", 8192);
	
	printSHMTab(&shm);
	
	char text[1024];
	fgets(text, 1024, stdin);
}
