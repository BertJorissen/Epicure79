
IOLIB = iohb.o
CFLAGS = -O3

all: $(IOLIB)
	ar rcs libiohb.a iohb.o

.c.o:
	$(CC) $(CFLAGS) -c $*.c

clean:
	rm -Rf libiohb.a iohb.o
