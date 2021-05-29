#ifndef SORTING_H
#define SORTING_H

#include <mpi.h>

/**
 * Parallel bitonic sort
 *
 * See https://en.wikipedia.org/wiki/Bitonic_sorter
 *
 * @param local_data  Local portion of array containing data to be sorted
 * @param n           Total size of distributed array
 * @param comm        MPI communicator
 */
void bitonic_sort(int* local_data, int n, MPI_Comm comm);


/**
 * Parallel sample sort
 *
 * See https://en.wikipedia.org/wiki/Samplesort. The number of local elements
 * after sorting is not necessarily the same before sorting. That's why a double
 * pointer is used, since local_sorted has memory allocated to it.
 *
 * @param local_data    Local portion of array containing data to be sorted
 * @param n             Total size of distributed array
 * @param local_sorted  Local output array containing sorted data
 * @param n_sort        Number of elements in output array (determined at runtime)
 * @param comm          MPI communicator
 */
void sample_sort(int* local_data, const int n, int** local_sorted, int& n_sort,
		 MPI_Comm comm);


/**
 * Merges two sorted arrays such that the output is sorted
 *
 * @param merge_buffer  Output buffer containing merged arrays
 * @param arr1          Array 1 of 2
 * @param n1            Size of array 1
 * @param arr2          Array 2 of 2
 * @param n2            Size of array 2
 */
inline void merge(int* merge_buffer, int* arr1, int n1, int* arr2, int n2)
{

    int i = 0;
    int j = 0;
    int k = 0;
    
    while (i < n1 or j < n2) {
	if (i < n1 and (j >= n2 or arr1[i] < arr2[j])) {
	    merge_buffer[k] = arr1[i];
	    i++;
	}
	else {
	    merge_buffer[k] = arr2[j];
	    j++;
	}
	k++;
    }
}

#endif
