CXX:=g++
CFLAGS:=-std=c++1z -Wall -Werror -pthread -I.

%.o: %.c ScopedAtomicAcquireRelease.h
	$(CXX) -c -o $@ $< $(CFLAGS)

lowlockqueue:  main.o ScopedAtomicAcquireRelease.o 
	$(CXX) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o

.PHONY: test
test:
	./lowlockqueue

