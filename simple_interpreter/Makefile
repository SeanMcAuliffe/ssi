# Makefile
# Sean McAuliffe, V00913346

# Compiler flags
CFLAGS = -Wall -std=c99

# Executable name
BIN = ssi

# Modules
OBJS = linked_list.o

.PHONY all:
all: $(BIN)

# This default rule compiles the executable program
$(BIN): $(OBJS) $(BIN).c
	$(CC) $(CFLAGS) $(OBJS) $(BIN).c -o $(BIN)

# Compile all modules
%.o: %.c %.h
	$(CC) -c $(CFLAGS) $(DEFS) $< -o $@

.PHONY clean:
clean:
	rm -f *.out *.o *.exe $(BIN)
