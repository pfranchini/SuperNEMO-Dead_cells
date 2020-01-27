CC = g++
CFLAGS = -DDEBUG -ggdb -Wall

all: dead_cells efficiency

dead_cells: dead_cells.cpp
		$(CC) $(CFLAGS) dead_cells.cpp -o dead_cells \
			-std=c++11 \
			-I./ \
			-I/usr/include/root/ \
			-I${ROOTSYS}/include \
		        `root-config --ldflags` \
	                `${ROOTSYS}/bin/root-config --glibs` \
	                -Wl,--no-as-needed \
		        -fdiagnostics-color \
		        -Wno-error

efficiency: efficiency.cpp
		$(CC) $(CFLAGS) efficiency.cpp -o efficiency  \
			-std=c++11 \
			-I./ \
			-I/usr/include/root/ \
			-I${ROOTSYS}/include \
		        `root-config --ldflags` \
	                `${ROOTSYS}/bin/root-config --glibs` \
	                -Wl,--no-as-needed \
		        -fdiagnostics-color \
		        -Wno-error


clean:
	rm -f dead_cells efficiency


