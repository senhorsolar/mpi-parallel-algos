#include "comm_primitives.h"
#include "utils.h"


void all_to_all(void* send_data, int* send_counts, void* recv_data, int* recv_counts,
		MPI_Datatype dtype, MPI_Comm comm)
{
    int size, rank;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    send_data = std::static_cast<char*>(send_data);
    recv_data = std::static_cast<char*>(recv_data);

    // Determine byte size of this mpi dtype
    int dtype_size;
    MPI_Type_size(dtype, &dtype_size);

    int* send_disps = new int[size];
    int* recv_disps = new int[size];

    // Determine where in memory the data being sent to rank i is at in
    // send/recv data. Assumes data is contiguous in the order of the given rank.
    int send_offset = 0;
    int recv_offset = 0;
    for (int i = 0; i < size; i++) {
	send_disps[i] = send_offset;
	recv_disps[i] = recv_offset;

	send_offset += dtype_size * send_counts[i];
	recv_offset += dtype_size * recv_counts[i];
    }

    for (int j = 1; j < size; j++) {
	// For minimal communication blocking
	int pair_rank = (rank + j) % size;
	
    	int send_idx = send_disps[pair_rank];
	int recv_idx = recv_disps[pair_rank];

	if (send_counts[j] != 0)
	    MPI_Send(&send_data[send_idx], send_counts[pair_rank], dtype, pair_rank, 111, comm);

	if (recv_counts[j] != 0)
	    MPI_Recv(&recv_data[recv_idx], recv_counts[pair_rank], dtype, pair_rank, 111, comm,
		     MPI_STATUS_IGNORE);
    }

    delete send_disps;
    delete recv_disps;
}
