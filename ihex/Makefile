CC = gcc
CFLAGS = -Wall
LDFLAGS =  


EXEC = ihex_parser
OBJS = parser.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)


clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
	-rm -f *.h~ *.c~

