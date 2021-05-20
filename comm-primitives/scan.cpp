#include "comm_primitives.h"
#include "utils.h"


double* scan(double* data, int n_local, std::function<double(double, double)> op, double id,
	     MPI_Comm comm)
{
    int size, rank;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    if (n_local == 0)
	return NULL;

    // Local scan
    double* local_result = new double[n_local];
    local_result[0] = data[0];
    for (int i = 1; i < n_local; i++)
	local_results[i] = op(local_results[i-1], data[i]);

    int d = log2_ceil(size);
    double prefix_sum = id;
    double total_sum = local_results[n_local-1];

    // Distributed scan
    for (int j = 0; j < d; j++) {
	int pair_rank = rank ^ (1 << j);

	if (pair_rank < size) {
	    double recv_sum;

	    MPI_Send(&total_sum, 1, MPI_DOUBLE, pair_rank, 111, comm);
	    MPI_Recv(&recv_sum, 1, MPI_DOUBLE, pair_rank, 111, comm, MPI_STATUS_IGNORE);

	    total_sum = op(recv_sum, total_sum);

	    if (rank > pair_rank)
		prefix_sum = op(recv_sum, prefix_sum);
	}
    }

    for (int i = 0; i < n_local; i++)
	local_results[i] = op(prefix_sum, local_results[i]);
    
    return local_results;
}
