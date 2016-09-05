#include <>

static void *find_fit(size_t asize) {
	char *bp = mem_heap + (2 * WSIZE);

	while (GET(HDRP(bp)) != 1) {
		bp = NEXT_BLKP(bp);
		if (GET_ALLOC(HDRP(bp)) == 0 &&
			GET_SIZE(HDRP(bp)) >= asize)
			return bp;
	}
	return NULL;
}
