CFLAGS=-std=c11 
LDFLAGS=-Wall -pthread
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

threadtest: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFlAGS) 

$(OBJS): tj.h

test: threadtest
	./threadtest

clean:
	rm -f *.o *~ tmp*

.PHONY: test clean
