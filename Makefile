# Part of libxil.
# Written by Edgar E. Iglesias
# GPLv2
#

CROSS?=microblaze-unknown-elf-
ENDIAN_FLAG?=

CC = $(CROSS)gcc
LD = $(CC)
AR = $(CROSS)ar

CFLAGS += -O2 -Wall -g -Wl,--no-relax $(ENDIAN_FLAG)

OBJS += xil.o
OBJS += xil-sbrk.o
OBJS += xil-signal.o

TARGET = libxil-linux.a

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) rcs $@ $^

clean:
	$(RM) $(OBJS) $(TARGET)
