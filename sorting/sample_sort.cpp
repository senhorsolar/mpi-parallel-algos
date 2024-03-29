#include "comm_primitives.h"
#include "sorting.h"
#include "utils.h"
#include <algorithm>


void merge_p(int* p_sorted_arrs, int p, int* sizes, int* displs, int* output)
{
    if (p == 1) {
	int n = sizes[0];
	for (int i = 0; i < n; i++) {
	    output[i] = p_sorted_arrs[i];
	}
	return;
    }

    int l = p / 2;
    int r = l + (p % 2);

    int left_size = displs[l];
    int right_size = displs[p-1] - left_size + 1;
    
    int* out1 = new int[left_size];
    int* out2 = new int[right_size];
    
    merge_p(p_sorted_arrs, l, sizes, displs, out1);
    merge_p(&p_sorted_arrs[left_size], r, &sizes[l], &displs[l], out2);

    merge(output, out1, left_size, out2, right_size);

    delete[] out1;
    delete[] out2;
}


void sample_sort(int* local_data, const int n, int** local_sorted, int& n_sort,
		 MPI_Comm comm)
{
    int size, rank;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    // Local sort
    int n_local = local_size(n, size, rank);
    std::sort(local_data, local_data + n_local);

    // Find local splitters
    int n_local_splitters = std::min(n_local, size-1);
    int* local_splitters = new int[n_local_splitters];

    int step = n_local / n_local_splitters;
    int offset = 0;

    for (int i = 0; i < n_local_splitters; i++) {
	local_splitters[i] = local_data[offset];
	offset += step;
    }

    // Determine total number of local splitters across each processor
    int total_local_splitters;
    MPI_Allreduce(&n_local_splitters, &total_local_splitters, 1, MPI_INT, MPI_SUM, comm);

    // Sort local splitters
    bitonic_sort(local_splitters, total_local_splitters, comm);

    // Obtain global splitters, which is the last local splitter on each processor
    int* global_splitters = new int[size];
    
    int* recv_counts = new int[size];
    int* recv_displs = new int[size];

    for (int i = 0; i < size; i++) {
	recv_counts[i] = 1;
	recv_displs[i] = i;
    }

    // After this all-gather, each processor has the global splitters
    MPI_Allgatherv(&local_splitters[n_local_splitters-1], 1, MPI_INT, global_splitters,
		   recv_counts, recv_displs, MPI_INT, comm);

    // Force the last element of the global splitters to be the largest element
    // in the local array. This is to avoid some data not being sent to the last
    // processor.
    global_splitters[size-1] = local_data[n_local-1];

    // Send fragments of local data to each other processor dependent on global splitter.
    // Determine how many will be sent to each rank
    int* send_counts = new int[size];
    for (int i = 0; i < size; i++) {
	int count = 0;
	while (offset < n_local and local_data[offset] <= global_splitters[i]) {
	    count += 1;
	}
	send_counts[i] = count;
    }

    // Determine how many elements will be sent/received to/from each processor
    int* ones = new int[size];
    std::fill(ones, ones + size, 1);
    all_to_all(send_counts, ones, recv_counts, ones, MPI_INT, comm);

    // Send/receive elements from each processor, according to how they are
    // fragmented by the global splitters.
    n_sort = 0;
    for (int i = 0; i < size; i++)
	n_sort += recv_counts[i];

    int* p_sorted_arrays = new int[n_sort];
    
    all_to_all(local_data, send_counts, p_sorted_arrays, recv_counts, MPI_INT, comm);

    // Merge the p sorted arrays
    *local_sorted = new int[n_sort];

    int* disps = recv_displs;
    offset = 0;
    for (int i = 0; i < size; i++) {
	disps[i] = offset;
	offset += recv_counts[i];
    }
    merge_p(p_sorted_arrays, size, recv_counts, disps, *local_sorted);

    delete[] p_sorted_arrays;
    delete[] ones;
    delete[] recv_counts;
    delete[] send_counts;
    delete[] recv_displs;
    delete[] local_splitters;
    delete[] global_splitters;
}
