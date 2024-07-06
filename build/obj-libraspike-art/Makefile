default: ../../lib/libraspike-art.a

SRC_DIR:=../../src

SRCS := raspike_protocol.c raspike_com.c raspike_color.c raspike_forcesensor.c raspike_motor.c \
	raspike_ultrasonicsensor.c raspike_hub.c

SPIKE_VERSION_FILE=$(SRC_DIR)/expected_spike_version



INCLUDES += -I$(SRC_DIR)
OPT=-g -O2
CLEAN_TARGETS=$(LIB_DIR)/$(LIB_NAME) 
COMPILE_LIB=1

include ../Makefile.common

VERSION=$(shell cat $(SPIKE_VERSION_FILE))

MAJOR := $(word 1,$(subst ., ,$(VERSION)))
MINOR := $(word 2,$(subst ., ,$(VERSION)))
PATCH := $(word 3,$(subst ., ,$(VERSION)))

CFLAGS+=-DSPIKE_EXPECTED_VERSION_MAJOR=$(MAJOR)  -DSPIKE_EXPECTED_VERSION_MINOR=$(MINOR) -DSPIKE_EXPECTED_VERSION_PATCH=$(PATCH)

$(OBJ_DIR)/raspike_protocol.o: $(SPIKE_VERSION_FILE)

versiontest:
	@echo $(VERSION)
	@echo $(MAJOR) $(MAJOR_BINARY)
	@echo $(MINOR) $(MINOR_BINARY)
	@echo $(PATCH) $(PATCH_BINARY)

../../lib/libraspike-art.a:$(addprefix $(OBJ_DIR)/,$(OBJS))
	ar rcs  $@ $^




