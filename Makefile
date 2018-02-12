CC := g++
CFLAGS := -Wall -std=c++1y

build: orcinusorca.cc
	$(CC) $(CFLAGS) -o orcinusorca orcinusorca.cc -lnetfilter_queue
clean:
	rm orcinusorca
