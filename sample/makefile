RASPIKE_LIB_ROOT=..

INCLUDES := -I$(RASPIKE_LIB_ROOT)/spike-rt/drivers/include -I$(RASPIKE_LIB_ROOT)/spike-rt/drivers -I$(RASPIKE_LIB_ROOT)/libpybricks/lib/pbio/include -I$(RASPIKE_LIB_ROOT) -I.

LIBS=$(RASPIKE_LIB_ROOT)/libraspike.a

default:raspike_test

clean:
	rm -f *~ *.o *.so *.a

raspike_test:raspike_test.o $(LIBS)
	gcc -g -O2 -o $@ $^ -pthread

.c.o:
	gcc -g -O2 -c $(INCLUDES) $(CFLAGS) -o $@ $<
