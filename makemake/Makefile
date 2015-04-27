# Makefile for usb_hid_io16 library
# for Linux
# (Quick'n'dirty - no .h dependencies, etc.)
# Assume make is run in this dir
# pa01 24-apr-2015

SRCDIR=../lib
VPATH = $(SRCDIR)

CMD_UTILITY := usb_hid_io16_cmd_test
SHRLIB      := usb_hid_io16.so

# For Linux: Using old simple version 0.1 of libusb
HIDDATA  := hiddata_libusb01
USBFLAGS := $(shell libusb-config --cflags)
USBLIBS  := $(shell libusb-config --libs)
EXE_SUFFIX=

#SRCS = usbrelay-cmd $(HIDDATA)

PROGRAM=	$(CMD_UTILITY)$(EXE_SUFFIX)

all: $(SHRLIB)

CC=		gcc
DEBUGFLAGS=
CFLAGS=		-O -Wall $(USBFLAGS) $(DEBUGFLAGS) -I$(SRCDIR)
LIBS=		$(USBLIBS)
OBJ=		$(addsuffix .o,$(SRCS))


$(PROGRAM): $(OBJ)
	$(CC) -o $(PROGRAM) $(OBJ) $(LIBS)

strip: $(PROGRAM)
	strip $(PROGRAM)

clean:
	rm -f *.o $(PROGRAM) $(SHRLIB)

%.c.o:
	$(CC) $(ARCH_COMPILE) $(CFLAGS) -c $*.c -o $*.o

#
# Shared library
# The main lib source and hiddata src are in [REPO]/lib/
#
SRCS_LIB = usb_io_interface $(HIDDATA)
CFLAGS += -I$(SRCDIR)

$(SHRLIB):	$(addsuffix .o, $(SRCS_LIB))
	$(CC) -shared -Wl,-shared $(addsuffix .o, $(SRCS_LIB)) $(LIBS) -o $(SHRLIB)


