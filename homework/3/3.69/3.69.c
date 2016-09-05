typedef struct ELE *tree_ptr;

struct ELE{
	tree_ptr left;
	tree_ptr right;
	long val;
};

long trace(tree_ptr tp)
{
	long result = 0;
	while(tp != NULL)
	{
		result = tp -> val;
		tp = tp -> left;
	}
	return result;
}

/*B : return the most left element of the binary-tree*/
