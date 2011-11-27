
GCC=gcc
CFLAGS=-O3 -g -fexceptions

%.o: %.c
	$(GCC) $(CFLAGS) -c $<

%.o: %.S
	$(GCC) $(CFLAGS) -c $<

