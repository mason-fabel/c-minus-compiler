CC := gcc
CFLAGS := -DYYDEBUG
BIN := c-
SRC := scanner.l parser.y
GEN := scanner.c parser.c parser.h
OBJ := parser.o scanner.o getopt.o

.PHONY : all clean test

$(BIN) : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

scanner.c : scanner.l parser.h token.h
	flex --outfile=scanner.c scanner.l

parser.c parser.h : parser.y token.h getopt.h
	bison --defines=parser.h --output=parser.c parser.y

%.o : $.c
	$(CC) $(CFLAGS) -c $<

all :
	touch $(SRC)
	make $(BIN)

clean :
	rm -f $(BIN) $(OBJ) $(GEN)

test : $(BIN)
	./c- test/scannerTest.c- > out.txt
	cat test/scannerTest.out > good.txt
	diff --text --side-by-side out.txt good.txt | less
	rm out.txt good.txt

tar : 
	tar -cf fabe0940.tar README makefile scanner.l parser.y getopt.c token.h
