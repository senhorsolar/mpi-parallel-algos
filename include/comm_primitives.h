#pragma once

#include <functional>
#include <mpi.h>

/**
 * Broadcasts data from a root processor to all other processors.
 *
 * This is a reimplementation of MPI_Bcast for educational purposes.
 */
void broadcast(void* data, int count, MPI_Datatype dtype, int root, MPI_Comm comm);

/**
 * Reduces data (of type double) across all processors.
 *
 * Performs an all-reduce, where each processor ends up with the total reduction.
 */
double reduce(double* data, int n, std::function<double(double, double)> op, MPI_Comm comm);

/**
 * Also known as parallel prefix.
 */
double* scan(double* data, int n, std::function<double(double, double)> op, double id,
	     MPI_Comm comm);

// TODO
// void gather(void* data, int n, MPI_Datatype dtype, int root, MPI_Comm comm);

// TODO
// void scatter(void* data, int n, MPI_Datatype dtype, int root, MPI_Comm comm);

// TODO
// void all_to_all();

// TODO
// void many_to_many();
