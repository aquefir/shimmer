#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "../src/shimmer.h"


int main(){
	printf("View program\n");
	
	SHMTab* shtab = connectShimmerTab("shmem", 16384);
	
	printf("%p\n", shtab);
	if(shtab != NULL){
		printf("%s\n", getFilename(shtab));
	}
}
