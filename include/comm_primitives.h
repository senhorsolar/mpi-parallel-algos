#ifndef COMM_PRIMITIVES_H
#define COMM_PRIMITIVES_H


#include <functional>
#include <mpi.h>

/**
 * Broadcasts data from a root processor to all other processors.
 *
 * @param data   Data to broadcast
 * @param count  How many elements of the given type the data contains
 * @param dtype  MPI datatype of elements
 * @param root   Root rank (such as 0)
 * @param comm   MPI communicator
 */
void broadcast(void* data, int count, MPI_Datatype dtype, int root, MPI_Comm comm);


/**
 * Reduces data (of type double) across all processors.
 *
 * Performs an all-reduce, where each processor ends up with the total reduction.
 * Each processor has an array of size n_local. The reduction first happens
 * locally, then it happens across processors.
 *
 * @param data     Local data on processor
 * @param n_local  Size of local data
 * @param op       Associative operator
 * @param comm     MPI communicator
 * @return The reduction across all processors
 */
double reduce(double* data, int n_local, std::function<double(double, double)> op, MPI_Comm comm);


/**
 * Performs a parallel prefix operation across all processors
 *
 * Given data in the form of A[i] for i = 1...n, and a prefix operator op(a, b),
 * performs R[i] = op(id, op(A[0],....op(A[i-1], A[1]))), where R[i] is the
 * result. The element _id_ is the initial value.
 *
 *
 * @param data     Local data on processor
 * @param n_local  Size of local data
 * @param op       Associative operator
 * @param id       Initial value
 * @param comm     MPI communicator
 * @return Array of local prefix results
 */
double* scan(double* data, int n_local, std::function<double(double, double)> op, double id,
	     MPI_Comm comm);

/**
 * Performs a gather, so that the local data on each processor is combined
 * and received on the root rank.
 *
 * To handle arbitrary data types, we work with byte (char) arrays. The type
 * size is inferred from the MPI datatype.
 *
 * @param gathered_data  Where the combined data is stored
 * @param n              Total number of _bytes_ to be gathered
 * @param local_data     Local data to be sent
 * @param n_local        Number of bytes in local data
 * @param dtype          MPI datatype
 * @param root           Rank that receives the data
 * @param comm           MPI communicator
 */
void gather(void* gathered_data, const int n, const void* local_data, const int n_local,
	    MPI_Datatype dtype, int root, MPI_Comm comm);

/**
 * Scatters data stored on the root rank to all other processors.
 *
 * The number of elements is spread as evenly as possible across all processors.
 * Like gather, we work with byte arrays to handle arbitrary types.
 *
 * @param scattered_data  Data to be scattered. Non null only on root.
 * @param n               Number of elements to be scattered
 * @param local_data      Local data array to receive partitioned data
 * @param n_local         Number of elements to be received at current rank
 * @param dtype           MPI datatype of elements
 * @param root            Rank which does the scattering
 * @param comm            MPI communicator
 */
void scatter(void* scattered_data, const int n, void* local_data, const int n_local,
	     MPI_Datatype dtype, int root, MPI_Comm comm);

/**
 * Each processor sends data to each other processor 
 *
 * @param send_data    The local data which gets distributed to the other
 *                     processors
 * @param send_counts  Array containing the number of elements to send to each
 *                     processor
 * @param recv_data    Where to store the data received from each processor
 * @param recv_counts  How much data is being received from each processor
 * @param dtype        MPI datatype of both sent/received data
 * @param comm         MPI communicator 
 */
void all_to_all(void* send_data, int* send_counts, void* recv_data, int* recv_counts,
		MPI_Datatype dtype, MPI_Comm comm);

#endif
