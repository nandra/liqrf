CC = gcc
CFLAGS = -Wall
LDFLAGS = -lusb


EXEC = liqrf
OBJS = usb.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)


clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

