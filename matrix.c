#include "matrix.h"
#include <stdlib.h>
#include <string.h>

typedef struct _matrix {
	unsigned row;
	unsigned col;
	unsigned size;
	void*    data;
} matrix;

static int matrix_errno__ = 0;


static void matrix_get_errno(int* merrno)
{
	*merrno = matrix_errno__;
}

static void matrix_set_errno(int merrno)
{
	matrix_errno__ = merrno;
}

inline static matrix* matrix_cast(matrix_t m)
{
	return (matrix*)m;
}

inline static unsigned matrix_row(matrix_t m)
{
	return matrix_cast(m)->row;
}

inline static unsigned matrix_col(matrix_t m)
{
	return matrix_cast(m)->col;
}

inline static unsigned matrix_size(matrix_t m)
{
	return matrix_cast(m)->size;
}

inline static void* matrix_data(matrix_t m)
{
	return matrix_cast(m)->data;
}

inline static unsigned matrix_cell_index(matrix_t m, unsigned row, unsigned col)
{
	return matrix_col(m) * row + col;
}

inline static unsigned matrix_byte_index(matrix_t m, unsigned row, unsigned col)
{
	return matrix_cell_index(m, row, col) * matrix_size(m);
}

inline static cell_t matrix_cell(matrix_t m, unsigned row, unsigned col)
{
	void* data = matrix_cast(m)->data;
	char* array = (char*)data;
	char* cell = array + matrix_byte_index(m, row, col);

	return (cell_t)cell;
}

inline static int matrix_check_get_set(matrix_t m, unsigned row, unsigned col)
{
	// Checks row
	if (row >= matrix_row(m)) {
		matrix_set_errno(matrix_error_parameter);
		return -1;
	}

	// Checks col
	if (row >= matrix_col(m)) {
		matrix_set_errno(matrix_error_parameter);
		return -1;
	}

	return 0;
}

inline static int matrix_check_add_sub_copy(matrix_t m1, matrix_t m2)
{
	// Checks row
	if (matrix_row(m1) != matrix_row(m2)) {
		matrix_set_errno(matrix_error_mismatch);
		return -1;
	}

	// Checks cow
	if (matrix_col(m1) != matrix_col(m2)) {
		matrix_set_errno(matrix_error_mismatch);
		return -1;
	}

	// Checks size
	if (matrix_size(m1) != matrix_size(m2)) {
		matrix_set_errno(matrix_error_mismatch);
		return -1;
	}

	return 0;
}

matrix_t matrix_create_internal(unsigned row, unsigned col, unsigned size)
{
	// Allocates matrix information
	matrix* m = (matrix*)malloc(sizeof(matrix));
	if (!m) {
		matrix_set_errno(matrix_error_alloc);

		return NULL;
	}

	// Allocates matrix cells
	void* data = malloc(row * col * size);
	if (!data) {
		free(m);
		matrix_set_errno(matrix_error_alloc);

		return NULL;
	}

	// Assigns the values
	m->row = row;
	m->col = col;
	m->size = size;
	m->data = data;

	return (matrix_t)m;
}

matrix_t matrix_create(unsigned row, unsigned col, unsigned size, u_op_t init)
{
	// Creates matrix
	matrix_t m = matrix_create_internal(row, col, size);
	if (!m)
		return NULL;

	// Initializes the cells
	if (init) {
		for (unsigned i = 0; i < row; ++i) {
			for (unsigned j = 0; j < col; ++j) {
				cell_t cell = matrix_cell(m, i, j);
				init(cell);
			}
		}
	}
	
	return m;
}

matrix_t matrix_copy(matrix_t dest, const matrix_t src)
{
	int check = matrix_check_add_sub_copy(dest, src);
	if (check)
		return NULL;

	unsigned row = matrix_row(dest);
	unsigned col = matrix_col(dest);
	unsigned size = matrix_size(dest);

	// Initializes the cells
	for (unsigned i = 0; i < row; ++i) {
		for (unsigned j = 0; j < col; ++j) {
			cell_t dest_cell = matrix_cell(dest, i, j);
			cell_t src_cell = matrix_cell(dest, i, j);
			memcpy(dest_cell, src_cell, size);
		}
	}

	return dest;
}

inline static matrix_t matrix_create_from_matrix(matrix_t m, u_op_t init)
{
	return matrix_create(
		matrix_row(m),
		matrix_col(m),
		matrix_size(m),
		init
	);
}

matrix_t matrix_clone(const matrix_t m)
{
	matrix_t c = matrix_create_from_matrix(m, NULL);
	if (!c)
		return NULL;

	return matrix_copy(c, m);
}

matrix_t matrix_add(matrix_t m1, matrix_t m2, b_op_t add)
{
	int check = matrix_check_add_sub_copy(m1, m2);
	if (check)
		return NULL;

	matrix_t result = matrix_create_from_matrix(m1, NULL);
	if (!result)
		return NULL;

	unsigned row = matrix_row(result);
	unsigned col = matrix_col(result);

	for (unsigned i = 0; i < row; ++i) {
		for (unsigned j = 0; j < col; ++j) {
			cell_t ds = matrix_cell(result, i, j);
			cell_t v1 = matrix_cell(m1, i, j);
			cell_t v2 = matrix_cell(m2, i, j);
			add(ds, v1, v2);
		}
	}

	return result;
}

matrix_t matrix_sub(matrix_t m1, matrix_t m2, b_op_t sub)
{
	return matrix_add(m1, m2, sub); // :)
}

matrix_t matrix_mul(matrix_t m1, matrix_t m2, b_op_t add, b_op_t mul)
{

}

void matrix_destroy(matrix_t m)
{
	free(matrix_data(m));
	free(m);
}

void matrix_print(matrix_t m, u_op_t print)
{
	unsigned row = matrix_row(m);
	unsigned col = matrix_col(m);

	for (unsigned i = 0; i < row; ++i) {
		for (unsigned j = 0; j < col; ++j) {
			if (j)
				printf(", ");
			cell_t cell = matrix_cell(m, i, j);
			print(cell);
		}

		printf("\n");
	}
}

int matrix_errno()
{
	return matrix_errno__;
}

int matrix_get(matrix_t m, unsigned row, unsigned col, cell_t cell)
{
	int check = matrix_check_get_set(m, row, col);
	if (check)
		return -1;

	memcpy(cell, matrix_cell(m, row, col), matrix_size(m));

	return 0;
}

int matrix_set(matrix_t m, unsigned row, unsigned col, cell_t cell)
{
	int check = matrix_check_get_set(m, row, col);
	if (check)
		return -1;

	memcpy(matrix_cell(m, row, col), cell, matrix_size(m));

	return 0;
}
