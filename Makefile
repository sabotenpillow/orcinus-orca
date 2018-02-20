CC     = clang++
CFLAGS = -Wall -std=c++1y -stdlib=libc++ -g -O0
LIB    = -lnetfilter_queue -lncurses -lboost_system -lboost_thread
#CC := g++
#CFLAGS := -Wall -std=c++1y
SRC = orcinusorca.cc nfq.cc ncurses.cc
OBJ = $(SRC:.cc=.o)

build: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o orcinusorca $(LIB)
.cc.o:
	$(CC) $(CFLAGS) $< -c -o $@
clean:
	rm orcinusorca *.o
