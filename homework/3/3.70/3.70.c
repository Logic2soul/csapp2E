#define NULL 0

typedef struct ELE *tree_ptr;

struct ELE{
	tree_ptr left;
	tree_ptr right;
	long val;
};

long traverse(tree_ptr tp)
{
	if(tp == NULL)
		return 0x7fffffffffffffff;
	else
	{
		long left_min = traverse(tp->left);
		long right_min = traverse(tp->right);
		return (left_min <= right_min)? left_min : right_min;
	}
}

