default: ../../lib/libraspike-art.a

SRC_DIR:=../../src

SRCS := raspike_protocol.c raspike_com.c raspike_color.c raspike_forcesensor.c raspike_motor.c \
	raspike_ultrasonicsensor.c raspike_hub.c

INCLUDES += -I$(SRC_DIR)
OPT=-g -O2
CLEAN_TARGETS=$(LIB_DIR)/$(LIB_NAME) 
COMPILE_LIB=1

include ../Makefile.common

../../lib/libraspike-art.a:$(addprefix $(OBJ_DIR)/,$(OBJS))
	ar rcs  $@ $^




