CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lusb -lrt 


EXEC = liqrf
OBJS = usb.o hex_parser.o spi.o liqrf.o iqrf.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)


clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
	-rm -f *.h~ *.c~

