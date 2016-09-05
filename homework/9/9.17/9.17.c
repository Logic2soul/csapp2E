#include <csapp.h>
#include <config.h>
#include <memlib.h>
#include <mm.h>

static void *find_fit(size_t asize) {
	char *bp = mem_nextfit;

	while (GET(HDRP(bp)) != 1) {
		bp = NEXT_BLKP(bp);
		if (GET_ALLOC(HDRP(bp)) == 0 &&
				GET_SIZE(HDRP(bp)) >= asize) {
			mem_nextfit = bp;
			return bp;
		}
	}
	return NULL;
}

static void place(void *bp, size_t asize) {
	size_t left;

	if ((left = GET_SIZE(HDRP(bp)) - asize) >= 4 * WSIZE) {
		GET(HDRP(bp)) = asize + 1;
		GET(FTRP(bp)) = asize + 1;
		GET(HDRP(NEXT_BLKP(bp))) = left;
		GET(FTRP(NEXT_BLKP(bp))) = left;
	}
	else {
		GET(HDRP(bp)) | 0x1;
		GET(FTRP(bp)) | 0x1;
	}
	return;
}

int main(void) {
	char *p;

	mem_init();
	mm_init();
	p = (char *)mm_malloc(10);
	mm_free(p);
	printf("ok\n");
	exit(0);
}
