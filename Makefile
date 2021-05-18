COMM_PRIMATIVES = $(shell ls comm-primitives/*.cpp)

CPPFILES = $(COMM_PRIMATIVES)

OFILES = $(CFILES:.cpp=.o)

IDIR = include
