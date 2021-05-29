CXX=mpic++
CCFLAGS = -Wall -O3 -std=c++17 -fPIC

# All the C++ files
COMM_PRIMATIVES = $(shell ls comm_primitives/*.cpp)
PARALLEL_PREFIX = $(shell ls parallel_prefix/*.cpp)
SORTING = $(shell ls sorting/*.cpp)
MATRIX = $(shell ls matrix/*.cpp)

CPPFILES = $(COMM_PRIMATIVES) $(PARALLEL_PREFIX) $(SORTING) $(MATRIX)

OFILES = $(CPPFILES:.cpp=.o)

# Include directory
IDIR = include

LIBNAME = libmpialgos

default: $(OFILES)
	ar rs $(LIBNAME).a $(OFILES)
	$(CXX) -shared $(OFILES) -o $(LIBNAME).so

%.o: %.cpp
	$(CXX) $(CCFLAGS) -I$(IDIR) -c $< -o $@

clean:
	rm -f */*.o
	rm -f *.o
	rm -f $(LIBNAME).a
	rm -f $(LIBNAME).so
