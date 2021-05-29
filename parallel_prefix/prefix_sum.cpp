#include "parallel_prefix.h"


void prefix_sum(void* local_data, const int n, MPI_Datatype dtype, MPI_Comm comm)
{
    MPI_Scan(local_data, local_data, n, dtype, MPI_SUM, comm);
}
