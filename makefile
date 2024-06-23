defailt: sample
#default:libraspike.a

INCLUDES := -I. -Ispike-rt/drivers/include -Ispike-rt/drivers/include/spike/pup -Ispike-rt/drivers/include/spike/hub -Ispike-rt/drivers -Ilibpybricks/lib/pbio/include/pbio  -Ilibpybricks/lib/pbio/include 

SRCS := raspike_protocol.c raspike_com.c raspike_color.c raspike_forcesensor.c raspike_motor.c \
	raspike_ultrasonicsensor.c raspike_hub.c
OBJS := $(SRCS:.c=.o)

libraspike.a:$(OBJS)
	ar rcs $@ $^

clean:
	rm -f *~ *.o *.so *.a sample/*.o raspike_test

sample:raspike_test

raspike_test:sample/raspike_test.o libraspike.a
	gcc -g -O2 -o $@ $(INCLUDES) $^ -pthread


.c.o:
	gcc -g -O2 -c $(INCLUDES) $(CFLAGS) -o $@ $<
