CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -O2
BIN := c-
SRC := scanner.l parser.y
GEN := scanner.c parser.c parser.h
OBJ := parser.o scanner.o

.PHONY : all clean

$(BIN) : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

scanner.c : scanner.l parser.h
	flex --outfile=scanner.c scanner.l

parser.c parser.h : parser.y
	bison --defines=parser.h --output=parser.c parser.y

%.o : $.c
	$(CC) $(CFLAGS) -c $<

all :
	touch $(SRC)
	make $(BIN)

clean :
	rm -f $(BIN) $(OBJ) $(GEN)
