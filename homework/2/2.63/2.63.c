#include <stdio.h>
#include <stdlib.h>

#define w (8 * sizeof(int))

typedef unsigned char* byte_pointer;

void show_bytes(byte_pointer start, int len)
{
        int i;
        for(i = 0; i < len; i++){
                printf("%.2x ", start[i]);
        }
        printf("\n");
}

int sra(int x, int k)
/*使用逻辑右移及位级运算完成算术右移*/
{
	int xsrl = (unsigned) x >> k;
/*对有符号整数xsrl强制逻辑右移k位*/
	int y = xsrl & ( 1 << (w - 1 - k));
/*将有符号整数xsrl的除符号位的其它位置为0*/
	return xsrl | ((y << (k + 1)) - (y << 1));
/*将xsrl逻辑右移扩展的高位0全置为xsrl的符号位0或1*/
}

unsigned srl(unsigned x, int k)
/*使用算术右移及位级运算完成逻辑右移*/
{
	unsigned xsra = (int) x >> k;
/*对无符号整数xsra强制算术右移k位*/
	return xsra & (~ (0 - (1 << (w - k))));
/*对算术右移后的xsra的位级进行 & 0x000..(前k位）1111111..... 操作*/	
}

int main(int argc, char** argv)
{
	int x;
	unsigned ux;
	int k;
	if(argc != 4)
	{
		fprintf(stderr, "usage: %s <int> <unsigned> <int>\n", argv[0]);
		exit(0);
	}
	sscanf(argv[1], "%d", &x);
	sscanf(argv[2], "%u", &ux);
	sscanf(argv[3], "%d", &k);
	show_bytes((byte_pointer)&x, sizeof(int));
	x =sra(x, k);
	show_bytes((byte_pointer)&x, sizeof(int));
        show_bytes((byte_pointer)&ux, sizeof(unsigned));
        ux =srl(ux, k);
        show_bytes((byte_pointer)&ux, sizeof(unsigned));
	exit(0);
}

