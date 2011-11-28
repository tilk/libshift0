
GCC=gcc
GPP=g++
CFLAGS=-O3 -g -fexceptions
CPPFLAGS=-O3 -g -fno-inline
OBJS=shift0.o shift0ops.o shift0personality.o
HEADERS=shift0.h shift0.hpp

%.o: %.c $(HEADERS)
	$(GCC) $(CFLAGS) -c $<

%.o: %.cpp $(HEADERS)
	$(GPP) $(CPPFLAGS) -c $<

%.o: %.S $(HEADERS)
	$(GCC) $(CFLAGS) -c $<

libshift0.a: $(OBJS)
	ar rcs libshift0.a $(OBJS)

all: libshift0.a

tests: libshift0.a tests.o
	$(GPP) tests.o libshift0.a -o tests

clean: 
	rm $(OBJS) tests.o libshift0.a

