#include "comm_primitives.h"
#include "utils.h"
#include <cstring>


void gather(void* vgathered_data, const int n, const void* local_data, const int n_local, MPI_Datatype dtype, int root, MPI_Comm comm)
{
    int size, rank;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    if (rank != root) { // Send data to root
	MPI_Send(local_data, n_local, dtype, root, 111, comm);
    }
    else { // Root receives data from each processor

	// Determine dtype's byte size
	int dtype_size;
	MPI_Type_size(dtype, &dtype_size);

	// Cast to char* so that we can do pointer arithmetic
	char* gathered_data = (char*)vgathered_data;
	
	for (int i_rank = 0; i_rank < size; i_rank++) {

	    // Size of local data for i_rank
	    int i_rank_n = local_size(n, size, i_rank);

	    // Root already has its data
	    if (i_rank == root) {		
		memcpy(gathered_data, local_data, n_local * dtype_size);
	    }
	    else {
		MPI_Recv(gathered_data, i_rank_n, dtype, i_rank, 111, comm, MPI_STATUS_IGNORE);
	    }

	    gathered_data += (i_rank_n * dtype_size);
	}
    }
}
