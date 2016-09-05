int loop(int x, int n)
{
	int result = 0x55555555;
	int mask;
	for(mask = 0x80000000; mask != 0; mask = (mask >> (n & 0xFF)) & ((1 << (n & 0xFF)) - 1))
	{
		result ^= (x & mask);
	}
	return result;
}

