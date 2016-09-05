#include <>

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
