#ifndef UTILS_H
#define UTILS_H


inline bool ispow2(int x)
{
    if (x == 0) {
	return false;
    }
    else {
	return (0 == (x & (x-1)));
    }
}

inline int log2_floor(int p)
{

    if (p <= 0)
	return -1;

    int d = 0;

    while ((p >>= 1) > 0)
	d++;

    return d;
}

inline int log2_ceil(int p)
{
    if (ispow2(p)) {
	return log2_floor(p);
    }
    else {
	return 1 + log2_floor(p);
    }	
}

inline int local_size(int n, int size, int rank)
{
    return n / size + (rank < n % size);
}

#endif
