
IOLIB = matrix_io.o
CFLAGS += -O3

all: $(IOLIB)
	ar rcs libmatrix_io.a matrix_io.o

.c.o:
	$(CC) $(CFLAGS) -c $*.c

clean:
	rm -Rf libmatrix_io.a matrix_io.o
