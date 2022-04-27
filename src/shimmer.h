#ifndef __SHIMMER_HEADER__
#define __SHIMMER_HEADER__


#include "stdint.h"


struct shm_dbuf{
	uint64_t	tag;
	uint8_t*	bufferA;
	uint8_t*	bufferB;
	
	uint32_t	size, mode, lock;
};

struct shm_circ{
	uint64_t	tag;
	uint8_t*	buffer;
	uint32_t	size, head, tail, lock;
};

struct shm_bkpt{
	uint64_t	tag;
	uint8_t		lock;
};


/*
 * Context is a struct that gets allocated to address 0xCABBA9E00.
 * It stores a data structure for indexing shmem units.
 * If it is not found, or if it has an invalid magic constant,
 * shimmer initialization fails and returns an error.
 */
struct shimmer_context{
	uint64_t	magic;
	uint32_t	size;
};


/* initialization */
int init_shimmer  (uint64_t, uint32_t);
int init_shm_dbuf (uint64_t, uint32_t);
int init_shm_circ (uint64_t, uint32_t);
int init_shm_bkpt (uint64_t);

/* double buffer operations */
int write_shm_dbuf(struct shm_dbuf*, uint32_t, uint32_t, void*);
int  flip_shm_dbuf(struct shm_dbuf*);
int  read_shm_dbuf(struct shm_dbuf*, uint32_t, uint32_t, void*);
int  lock_shm_dbuf(struct shm_dbuf*);

/* circular buffer operations */
int write_shm_circ(struct shm_circ*, uint32_t, void*);
int  read_shm_circ(struct shm_circ*, uint32_t, void*);
int  lock_shm_circ(struct shm_circ*);

/* breakpoint operations */
int  lock_shm_bkpt(struct shm_bkpt*);






#endif
