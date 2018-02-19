CC     = clang++
CFLAGS = -Wall -std=c++1y -stdlib=libc++
LIB    = -lnetfilter_queue -lncurses
#CC := g++
#CFLAGS := -Wall -std=c++1y
SRC = orcinusorca.cc nfq.cc
OBJ = $(SRC:.cc=.o)

build: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o orcinusorca $(LIB)
.cc.o:
	$(CC) $(CFLAGS) $< -c -o $@
clean:
	rm orcinusorca *.o
