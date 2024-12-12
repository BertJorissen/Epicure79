export CFLAGS += -fPIC

all:
	$(MAKE) -C metis
	$(MAKE) -C iohb1.0
	$(MAKE) -C matrix_io

clean:
	$(MAKE) -C metis realclean
	$(MAKE) -C iohb1.0 clean
	$(MAKE) -C matrix_io clean
