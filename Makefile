CC = gcc
CFLAGS = -Wall
LDFLAGS = -lusb -lrt 


EXEC = spi_example
OBJS = usb.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)


clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
	-rm -f *.h~ *.c~

