CC = gcc
CFLAGS = -Wall
LDFLAGS = -lusb -lrt 


EXEC = spi_example
OBJS = spi_example.o

EXEC1 = liqrf
OBJS1 = usb.o hex_parser.o spi.o liqrf.o iqrf.o

all: $(EXEC) $(EXEC1)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

$(EXEC1): $(OBJS1)
	$(CC) $(LDFLAGS) -o $@ $(OBJS1)
clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
	-rm -f $(EXEC1) *.elf *.gdb *.o
	-rm -f *.h~ *.c~

