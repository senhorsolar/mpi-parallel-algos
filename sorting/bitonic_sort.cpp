#include "sorting.h"
#include "utils.h"
#include <algorithm>


void bitonic_sort(int* local_data, int n, MPI_Comm comm)
{
    int size, rank;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    // Sort local elements
    int n_local = local_size(n, size, rank);
    std::sort(local_data, local_data + n_local);

    int n_max = n / size + (n % size > 0);
    int* recv_buffer = new int[n_max];
    int* merge_buffer = new int[n_max * 2];
 
    int d = log2_ceil(size);
    for (int i = 0; i < d; i++) {
	
	int ith_bit_mask = 1 << i;
	
	for (int j = i; j >= 0; j--) {
	    
	    int pair_rank = rank ^ (1 << j);
	    
	    if (pair_rank < size) {
		
		int n_pair = local_size(n, size, pair_rank);
	    
		MPI_Send(&local_data, n_local, MPI_INT, pair_rank, 111, comm);
		MPI_Recv(&recv_buffer, n_pair, MPI_INT, pair_rank, 111, comm, MPI_STATUS_IGNORE);

		merge(merge_buffer, local_data, n_local, recv_buffer, n_pair);

		if (rank & ith_bit_mask) {
		    // Larger rank gets smaller elements
		    if (rank > pair_rank)
			memcpy(local_data, merge_buffer, n_local * sizeof(int));
		    else
			memcpy(local_data, merge_buffer + n_pair, n_local * sizeof(int));
		}
		else {
		    // Smaller rank gets smaller elements
		    if (rank < pair_rank)
			memcpy(local_data, merge_buffer, n_local * sizeof(int));
		    else
			memcpy(local_data, merge_buffer + n_pair, n_local * sizeof(int));
		}
	    }
	}
    }

    delete[] recv_buffer;
    delete[] merge_buffer;
}
