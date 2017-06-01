CC=g++
CFLAGS=-I.  -std=c++1z  -g -Wall -Werror -pthread
DEPS = LowLockQueue.h ScopedAtomicAcquireRelease.h
OBJ = main.o ScopedAtomicAcquireRelease.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o 

