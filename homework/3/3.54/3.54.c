int decode2(int x, int y, int z)
{
	return (x ^ (z - y)) * (((z - y) << 15) >> 15);
}

