#include "comm_primitives.h"
#include "utils.h"


double reduce(double* data, int n, std::function<double(double, double)> op, MPI_Comm comm)
{
    int size, rank;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);
    
    if (n == 0)
	return 0;
    
    double result = data[0];
    for (int i = 1; i < n; i++)
	result = op(data[i-1], data[i]);

    int d = log2_ceil(size);
    for (int j = 0; j < d; j++) {
	int pair_rank = rank ^ (1 << j);

	if (pair_rank < p) {
	    double recv_result;

	    MPI_Send(&result, 1, MPI_DOUBLE, pair_rank, 111, comm);
	    MPI_Recv(&recv_result, 1, MPI_DOUBLE, pair_rank, 111, comm, MPI_STATUS_IGNORE);
	    result = op(result, recv_result);
	}
    }

    return result;
}
