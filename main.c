#include "matrix.h"

void init_int(void* val)
{
	int* int_val = (int*)val;

	*int_val = 0;
}

void add_int(void* result, const void* val1, const void* val2)
{
	int* int_res = (int*)result;
	int* int_v1 = (int*)val1;
	int* int_v2 = (int*)val2;

	*int_res = *int_v1 + *int_v2;
}

int main()
{
	matrix_t m1 = matrix_create(3, 3, sizeof(int), init_int);
	matrix_t m2 = matrix_create(3, 3, sizeof(int), init_int);
	matrix_t m3 = matrix_add(m1, m2, add_int);

	return 0;
}
