#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "../src/shimmer.h"


int main(){
	printf("Test program\n");
	
	SHMTab* shtab =	initShimmerTab("shmem", 16384);
	printf("%p %s\n", shtab, getFilename(shtab));
	
	printSHMTab(shtab);
	
	char text[1024];
	fgets(text, 1024, stdin);
}
