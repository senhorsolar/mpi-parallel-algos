#pragma once

#include <mpi.h>

void merge(int* merge_buffer, int* arr1, int n1, int* arr2, int n2)
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
