int var_prod_ele(int n, int A[n][n], int B[n][n], int i, int k)
{
	char *Arow = (char *) A[i];
	char *Bcol = (char *) &B[0][k];
	int result = 0;
	int j;
	
	for(j = 0; j < 4*n; j += 4)
	{
		result +=(* (int*)(Arow + j)) * (* (int*)Bcol);
		(char *)Bcol;
		Bcol += (4*n);
		(char*)Arow;
	}
	return result;
}

