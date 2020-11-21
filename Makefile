name = erg3
src = $(wildcard *.cpp)
obj = $(src:/c=.o)

CC = g++
CFLAGS = -std=c++0x -O3
LIBFLAGS = -lleda -lm

Leda = '/usr/local/LEDA/incl'
LedaLibs = '/usr/local/LEDA'
BOOSTDIR = '/usr/include'
BOOSTINCL = '/usr/include/boost'
BOOSTLIB = '/usr/local/lib'

all: $(name)
$(name): $(obj)
	$(CC) $(CFLAGS) -o $@ $^ -I$(Leda) -L$(LedaLibs) $(LIBFLAGS) -I$(BOOSTINCL) -L$(BOOSTLIB)
	
run:
	./$(name)
	
clean:
	rm -f $(name)