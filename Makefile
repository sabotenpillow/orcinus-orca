CC := clang++
CFLAGS := -Wall -std=c++1y -stdlib=libc++
LIB := -lnetfilter_queue -lncurses
#CC := g++
#CFLAGS := -Wall -std=c++1y

build: orcinusorca.cc
	$(CC) $(CFLAGS) -o orcinusorca orcinusorca.cc $(LIB)
clean:
	rm orcinusorca
