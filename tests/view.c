#include "stdio.h"

#include "../src/shimmer.h"



int main(){
	Shimmer shm;
	if(!connectShimmer(&shm, "shmem", 256)){
		printf("Failed to connect to shimmer\n");
		return -1;
	}
	printf("SHM %p %li\n", shm.buffers[0], shm.sizes[0]);
	
	char* txt = shm.buffers[0];
	txt = &txt[sizeof(ShimmerContext)];
	printf("%s\n", txt);
}
