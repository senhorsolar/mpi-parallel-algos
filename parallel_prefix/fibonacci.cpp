#include "parallel_prefix.h"
#include "utils.h"


void matmul2x2(void *inbuffer, void* inoutbuffer, int* len, MPI_Datatype* dtype)
{
    using ulong = unsigned long long;
    
    ulong* in = (ulong*)inbuffer;
    ulong* out = (ulong*)inoutbuffer;

    ulong a11 = in[0] * out[0] + in[1] * out[2];
    ulong a12 = in[0] * out[1] + in[1] * out[3];
    ulong a21 = in[2] * out[0] + in[3] * out[2];
    ulong a22 = in[2] * out[1] + in[3] * out[3];

    out[0] = a11;
    out[1] = a12;
    out[2] = a21;
    out[3] = a22;
}

void matexp2x2(void* mat, int n)
{
    using ulong = unsigned long long;

    // 2x2 Identity matrix
    ulong* result = new ulong[4];
    result[0] = 1;
    result[1] = 0;
    result[2] = 0;
    result[3] = 1;
    
    while (n > 0) {
	if (n % 2)
	    matmul2x2(mat, result, NULL, NULL);

	matmul2x2(mat, mat, NULL, NULL);
	n /= 2;
    }

    memcpy(mat, result, sizeof(ulong) * 4);
    delete result;
}

unsigned long long fibonacci(unsigned int n, MPI_Comm comm)
{
    using ulong = unsigned long long;
    
    if (n <= 1)
	return (ulong)n;

    int size, rank;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);
    
    MPI_Op op;
    MPI_Op_create(&matmul2x2, 0, &op);

    int local_n = local_size(n, size, rank);

    ulong* mat = new ulong[4];
    mat[0] = 1;
    mat[1] = 1;
    mat[2] = 1;
    mat[3] = 0;

    if (rank == 0)
	local_n -= 1;

    matexp2x2(mat, local_n);
    
    MPI_Scan(mat, mat, 1, MPI_UNSIGNED_LONG_LONG, op, comm);

    ulong result = mat[0];
    
    MPI_Op_free(&op);

    return result;
}
