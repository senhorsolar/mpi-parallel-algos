#include "parallel_prefix.h"
#include <algorithm>


double polynomial_eval(const double* coefficients, const double x, const int n, MPI_Comm comm)
{
    int size, rank;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    double* xs = new double[n];
    std::fill(xs, xs + n, x);

    if (rank == 0)
	xs[0] = 1;

    double* powers_of_x = xs;
    MPI_Scan(xs, powers_of_x, n, MPI_DOUBLE, MPI_PROD, comm);

    // Inner summation
    double inner_sum = 0;
    for (int i = 0; i < n; i++)
	inner_sum += coefficients[i] * powers_of_x[i];

    // Outer summation
    double result;
    MPI_Allreduce(&inner_sum, &result, 1, MPI_DOUBLE, MPI_SUM, comm);

    delete xs;
    
    return result;
}
