#ifndef _MATRIX_H_
#define _MATRIX_H_

#ifdef __cplusplus
extern "C" {
#endif 


typedef void* matrix_t;
typedef void* cell_t;
typedef void(*u_op_t)(cell_t);
typedef void(*b_op_t)(cell_t result, const cell_t v1, const cell_t v2);

enum matrix_errno_t {
	matrix_error_alloc = 1,
	matrix_error_parameter = 2,
	matrix_error_mismatch = 3,
};


matrix_t matrix_create(unsigned row, unsigned col, unsigned size, u_op_t init);
matrix_t matrix_copy(matrix_t, const matrix_t);
matrix_t matrix_clone(const matrix_t);
matrix_t matrix_add(const matrix_t, const matrix_t, b_op_t add);
matrix_t matrix_sub(matrix_t, matrix_t, b_op_t sub);
matrix_t matrix_mul(matrix_t, matrix_t, b_op_t add, b_op_t mul);

void   matrix_destroy(matrix_t);
void   matrix_print(matrix_t, u_op_t print);
int    matrix_get(matrix_t, unsigned row, unsigned col, cell_t);
int    matrix_set(matrix_t, unsigned row, unsigned col, cell_t);
int    matrix_errno();


#ifdef __cplusplus
}
#endif 


#endif /* _MATRIX_H_ */
