#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "../src/shimmer.h"


int main(){
	printf("View program\n");
	
	SHM shm = connectShimmer("shmem", 128);
	
	printSHMTab(&shm);
	
	printf("%p\n", shm.shtab);
	if(shm.shtab != NULL){
		printf("%s\n", getFilename(shm.shtab));
	}
}
