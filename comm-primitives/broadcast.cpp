#include "comm_primitives.h"
#include "utils.h"


void broadcast(void* data, int count, MPI_Datatype dtype, int root, MPI_Comm comm)
{
    int size, rank;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    // Hypercubic permutation assumes data begins at rank 0
    if (root != 0) {
	if (rank == root) {
	    MPI_Send(data, count, dtype, 0, 111, comm);
	}
	else if (rank == 0) {
	    MPI_Recv(data, count, dtype, root, 111, comm, MPI_STATUS_IGNORE);
	}
    }

    int d = log2_ceil(size);
    int flip = 1 << (d-1);
    int mask = flip - 1;

    for (int j = d-1; j >= 0; j--) {

	int pair_rank = rank ^ flip;

	// Ignore communications with ranks higher than total available
	if (pair_rank < size) {
	    if ((rank & mask) == 0) {
		if ((rank & flip) == 0) { // Send
		    MPI_Send(data, count, dtype, pair_rank, 111, comm);
		}
		else { // Receive
		    MPI_Recv(data, count, dtype, pair_rank, 111, comm, MPI_STATUS_IGNORE);
		}
	    }
	}
	mask >>= 1;
	flip >>= 1;
    }
}
