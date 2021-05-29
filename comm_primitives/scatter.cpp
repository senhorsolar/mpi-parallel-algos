#include "comm_primitives.h"
#include "utils.h"
#include <cstring>


void scatter(void* vscattered_data, const int n, void* local_data, const int n_local,
	     MPI_Datatype dtype, int root, MPI_Comm comm)
{
    int size, rank;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    if (rank != root) { // Receive data from root
	MPI_Recv(local_data, n_local, dtype, root, 111, comm, MPI_STATUS_IGNORE);
    }
    else { // Root sends data to each processor
	int dtype_size;
	MPI_Type_size(dtype, &dtype_size);

	char* scattered_data = (char*)vscattered_data;

	for (int i_rank = 0; i_rank < size; i_rank++) {

	    int i_rank_n = local_size(n, size, i_rank);

	    // Root already has its data
	    if (i_rank == root) {
		memcpy(local_data, scattered_data, n_local * dtype_size);
	    }
	    else { // Send portion of data
		MPI_Send(scattered_data, i_rank_n, dtype, root, 111, comm);
	    }

	    scattered_data += (i_rank_n * dtype_size);
	}
    }
}
