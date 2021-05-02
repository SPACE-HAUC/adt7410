CC=gcc
RM=rm -vf

EDCFLAGS:= -O2 -I../ -Wall -DADT7410_UNIT_TEST $(CFLAGS) $(DEBUG)
EDLDFLAGS:= $(LDFLAGS) -lpthread -lm

COBJS=adt7410.o \
	../i2cbus/i2cbus.o

adt7410: $(COBJS)
	$(CC) -o $@.out $(COBJS) $(EDLDFLAGS)

%.o: %.c
	$(CC) $(EDCFLAGS) -o $@ -c $<

.PHONY: clean

clean:
	$(RM) $(COBJS)
	$(RM) *.out