OBJS	= scotchtest.o
SOURCE	= scotchtest.cpp
OUT	= scotchtest
CC  	= g++
FLAGS	= -c -Wall -pthread
LFLAGS	= -L$(SCOTCH_HOME)/lib -lscotch -lscotcherr -pthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

scotchtest.o: $(SOURCE)
	$(CC) $(FLAGS) $(SOURCE)

clean:
	rm -f $(OBJS) $(OUT)

test: all
	./scotchtest scotch_failures_20210611
