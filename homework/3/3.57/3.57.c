int cread_alt(int *xp)
{
	int *yp;
		*yp = 0;
	return *(xp ? xp : yp);
}

