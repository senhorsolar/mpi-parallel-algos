#ifndef PARALLEL_PREFIX_H
#define PARALLEL_PREFIX_H

#include <mpi.h>

/**
 * Performs a prefix sum across all processors.
 *
 * Given data in the form of A[i] for i = 1...n, 
 * performs R[i] = A[0] + ... + A[i], were R[i] is the result.
 * Takes advantage of MPI_Scan.
 * 
 * @param local_data  Local data to perform the prefix sum on
 * @param n           Size of local_data
 * @param dtype       MPI datatype
 * @param comm        MPI communicator
 */
void prefix_sum(void* local_data, const int n, MPI_Datatype dtype, MPI_Comm comm);


/**
 * Evaluates a polynomial where the coefficients are distributed across all processors
 * 
 * Evaluates P(x) = a(0) + a(1)*x + .... a(n-1)*x^(n-1).
 * 
 * @param coefficients  Array holding local coefficients
 * @param x             Where to evaluate polynomial on, i.e. x in P(x)
 * @param n_local       Size of coefficients (local size)             
 * @param comm          MPI communicator
 */
double polynomial_eval(const double* coefficients, const double x, const int n_local, MPI_Comm comm);


/**
 * Computes the fibonacci number fib(n) in a distributed fashion
 *
 * Uses a parallel prefix algorithm to compute fib(n), where
 *
 * [fib(n) fib(n-1)] = [1 0] * [1 1]^(n-1)
 *                             [0 1]
 * 
 * A parallel prefix algorithm is using matrix multiplication as the associative
 * operator.
 * 
 * @param n     Argument n in fib(n)
 * @param comm  MPI communicator
 */
unsigned long long fibonacci(unsigned int n, MPI_Comm comm);

#endif
