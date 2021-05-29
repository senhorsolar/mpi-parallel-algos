#include "matrix.h"
#include "utils.h"
#include <cmath>
#include <cstring>


void matvecprod(double* local_A, int n, double* local_x, double* local_y,
		MPI_Comm grid_comm)
{
    int rank, size;
    MPI_Comm_rank(grid_comm, &rank);
    MPI_Comm_size(grid_comm, &size);

    // Assume size is a perfect square
    int q = sqrt((double)size);

    int coords[2];
    MPI_Cart_coords(grid_comm, rank, 2, coords);

    // Dimensions of local A
    int nrow, ncol;
    nrow = local_size(n, q, coords[0]);
    ncol = local_size(n, q, coords[1]);

    double* local_x_T = new double[ncol];
    
    // If on the first column send local x to the diagonal
    if (coords[1] == 0) {

	if (coords[0] != 0) {
	    int diag_coords[] = {coords[0], coords[0]};
	    int diag_rank;
	    MPI_Cart_rank(grid_comm, diag_coords, &diag_rank);

	    MPI_Send(local_x, nrow, MPI_DOUBLE, diag_rank, 111, grid_comm);
	}

    }

    // If on diagonal rank
    if (coords[1] == coords[0]) {
	if (coords[0] == 0) {
	    memcpy(local_x_T, local_x, nrow * sizeof(double));
	}
	else {
	    int col_coords[] = {coords[0], 0};
	    int col_rank;
	    MPI_Cart_rank(grid_comm, col_coords, &col_rank);

	    MPI_Recv(local_x_T, ncol, MPI_DOUBLE, col_rank, 111, grid_comm,
		     MPI_STATUS_IGNORE);
	}
    }

    // Broadcast along column
    MPI_Comm column_comm;
    MPI_Comm_split(grid_comm, coords[1], coords[0], &column_comm);

    MPI_Bcast(local_x_T, ncol, MPI_DOUBLE, coords[1], column_comm);

    // Local matrix-vector product
    double* local_result = new double[nrow];

    for (int i = 0; i < nrow; i++) {
	local_result[i] = 0;
	int row_idx = i * ncol;
	for (int j = 0; j < ncol; j++) {
	    local_result[i] += local_A[row_idx + j] * local_x_T[j];
	}
    }

    // Summation reduce across row
    MPI_Comm row_comm;
    MPI_Comm_split(grid_comm, coords[0], coords[1], &row_comm);
    MPI_Reduce(local_result, local_y, nrow, MPI_DOUBLE, MPI_SUM, 0, row_comm);
    
    MPI_Comm_free(&column_comm);
    MPI_Comm_free(&row_comm);
    delete[] local_x_T;
    delete[] local_result;
}
