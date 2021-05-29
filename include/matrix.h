#ifndef MATRIX_H
#define MATRIX_H

#include <mpi.h>


/**
 * Performs a distributed matrix-vector product A*x = y
 *
 * The matrix A is assumed to be square and  2D block partitioned, and x and y
 * are assumed to be  block partitioned across the first column of the 2D grid.
 * The number of processors used is also assumed to be square.
 *
 * @param local_A    Local portion of A
 * @param n          Dimension of A, where A is an n-by-n matrix
 * @param local_x    Local portion of x, which is only on the first column
 * @param local_y    Local portion of y, which is only on the first column
 * @param grid_comm  MPI communicator on a 2D grid
 */
void matvecprod(double* local_A, int n, double* local_x, double* local_y,
		MPI_Comm grid_comm);


/**
 * Performs a distributed matrix-matrix multiplication with A*B = C
 *
 * A and B are assumed to be square, and the number of processors is assumed is
 * also assumed to be square. Even further, the dimension of A should be
 * divisible by the square root of the number of processors.
 * 
 * Uses Cannon's algorithm https://en.wikipedia.org/wiki/Cannon's_algorithm
 * Code was adopted from my course on parallel algorithms.
 *
 * @param local_A    Local portion of A
 * @param local_B    Local portion of B
 * @param local_C    Local portion of C
 * @param n          Dimension of A, where A is an n-by-n matrix
 * @param grid_comm  MPI communicator on a 2D grid
 */
void matmatprod(double* local_A, double* local_B, double* local_C, int n, MPI_Comm grid_comm);

#endif
