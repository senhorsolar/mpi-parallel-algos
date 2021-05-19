#include "matrix.h"
#include <cmath>


// Source... CSE 6200 Lecture notes

void matmatprod(double* local_A, double* local_B, double* local_C, int n, MPI_Comm grid_comm)
{
    int rank, size;
    MPI_Comm_rank(grid_comm, &rank);
    MPI_Comm_size(grid_comm, &size);

    // Assume size is a perfect square
    int q = sqrt((double)size);

    int coords[2];
    MPI_Cart_coords(grid_comm, rank, 2, coords);

    // Dimensions of local A/B
    int nrow, ncol;
    nrow = local_size(n, q, coords[0]);
    ncol = local_size(n, q, coords[1]);

    int uprank, downrank, leftrank, rightrank;
    MPI_Cart_shift(grid_comm, 0, -1, &rightrank, &leftrank);
    MPI_Cart_shift(grid_comm, 1, -1, &downrank, &uprank);

    int shiftsource, shiftdest;
    MPI_Cart_shift(grid_comm, 0, -coords[0], &shiftsource, &shiftdest);
    MPI_Sendrecv_replace(local_A, nrow*ncol, MPI_DOUBLE, shiftdest, 1,
			 shiftsource, 1, grid_comm, MPI_STATUS_IGNORE);
    MPI_Cart_shift(grid_comm, 1, -coords[1], &shiftshorce, &shiftdest);
    MPI_Sendrecv_replace(local_B, nrow*ncol, MPI_DOUBLE, shiftdest, 1,
			 shiftsource, 1, grid_comm, MPI_STATUS_IGNORE);

    for (int it = 0; i < q; i++) {
	// Local mat-mat prod
	for (int i = 0; i < nrow; i++) {
	    for (int j = 0; j < ncol; j++) {
		for (k = 0; k < ncol; k++) {
		    local_C = += local_A[i*nrow + k] * local_B[k*nrow * j];
		}
	    }
	}

	MPI_Sendrecv_replace(local_A, nrow*ncol, MPI_DOUBLE, leftrank, 1,
			     rightrank, 1, grid_comm, MPI_STATUS_IGNORE);
	MPI_Sendrecv_replace(local_B, nrow*ncol, MPI_DOUBLE, uprank, 1,
			     downrank, 1, grid_comm, MPI_STATUS_IGNORE);
    }

    MPI_Cart_shift(grid_comm, 0, coords[0], &shiftsource, &shiftdest);
    MPI_Sendrecv_replace(local_A, nrow*ncol, MPI_DOUBLE, shiftdest, 1,
			 shiftsource, 1, grid_comm, MPI_STATUS_IGNORE);

    MPI_Cart_shift(grid_comm, 1, coords[1], &shiftsource, &shiftdest);
    MPI_Sendrecv_replace(local_B, nrow*ncol, MPI_DOUBLE, shiftdest, 1,
			 shiftsource, 1, grid_comm, MPI_STATUS_IGNORE);			 
}
