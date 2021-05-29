#include "matrix.h"
#include "utils.h"
#include <cmath>


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
    int nlocal = local_size(n, q, coords[0]);

    // Determine ranks for shifting up/down left/right
    int uprank, downrank, leftrank, rightrank;
    MPI_Cart_shift(grid_comm, 0, -1, &rightrank, &leftrank);
    MPI_Cart_shift(grid_comm, 1, -1, &downrank, &uprank);

    int shiftsource, shiftdest;

    // Initial A alignment
    MPI_Cart_shift(grid_comm, 0, -coords[0], &shiftsource, &shiftdest);
    MPI_Sendrecv_replace(local_A, nlocal*nlocal, MPI_DOUBLE, shiftdest, 1,
			 shiftsource, 1, grid_comm, MPI_STATUS_IGNORE);

    // Initial B alignment
    MPI_Cart_shift(grid_comm, 1, -coords[1], &shiftsource, &shiftdest);
    MPI_Sendrecv_replace(local_B, nlocal*nlocal, MPI_DOUBLE, shiftdest, 1,
			 shiftsource, 1, grid_comm, MPI_STATUS_IGNORE);

    for (int it = 0; it < q; it++) {
	
	// Local mat-mat prod
	for (int i = 0; i < nlocal; i++) {
	    for (int j = 0; j < nlocal; j++) {
		for (int k = 0; k < nlocal; k++) {
		    local_C[i*nlocal + k] += local_A[i*nlocal + k] * local_B[k*nlocal * j];
		}
	    }
	}

	MPI_Sendrecv_replace(local_A, nlocal*nlocal, MPI_DOUBLE, leftrank, 1,
			     rightrank, 1, grid_comm, MPI_STATUS_IGNORE);
	MPI_Sendrecv_replace(local_B, nlocal*nlocal, MPI_DOUBLE, uprank, 1,
			     downrank, 1, grid_comm, MPI_STATUS_IGNORE);
    }

    // Restore A alignment
    MPI_Cart_shift(grid_comm, 0, coords[0], &shiftsource, &shiftdest);
    MPI_Sendrecv_replace(local_A, nlocal*nlocal, MPI_DOUBLE, shiftdest, 1,
			 shiftsource, 1, grid_comm, MPI_STATUS_IGNORE);

    // Restore B alignment
    MPI_Cart_shift(grid_comm, 1, coords[1], &shiftsource, &shiftdest);
    MPI_Sendrecv_replace(local_B, nlocal*nlocal, MPI_DOUBLE, shiftdest, 1,
			 shiftsource, 1, grid_comm, MPI_STATUS_IGNORE);			 
}
