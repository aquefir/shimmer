#include "stdio.h"

#include "../src/shimmer.h"



int main(){
	Shimmer shm;
	if(!initShimmer(&shm, "shmem", 256)){
		printf("Failed to initialize shimmer\n");
		return -1;
	}
	printf("SHM %p %li\n", shm.buffers[0], shm.sizes[0]);
	
	char* txt = shm.buffers[0];
	txt = &txt[sizeof(ShimmerContext)];
	strcpy(txt, "Hello outside world!\n");
	printf("%s\n", txt);
}
