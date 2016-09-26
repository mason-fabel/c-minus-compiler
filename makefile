CC := g++
CFLAGS := -Wall -Wextra -Wpedantic -DYYDEBUG
BIN := c-
GEN := scanner.c parser.c parser.h parser.output
OBJ := parser.o scanner.o getopt.o symbolTable.o

.PHONY : clean test

$(BIN) : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

scanner.c : scanner.l parser.h token.h
	flex --outfile=scanner.c scanner.l

parser.c parser.h : parser.y token.h getopt.h
	bison --verbose --report=all --defines=parser.h -Wall -Werror --output=parser.c parser.y

%.o : $.c
	$(CC) $(CFLAGS) -c $<

%.o : $.cpp
	$(CC) $(CFLAGS) -c $<

clean :
	rm -f $(BIN) $(OBJ) $(GEN)

test : $(BIN)
	./c- test/scannerTest.c- > out.txt
	cat test/scannerTest.out > good.txt
	diff --text --side-by-side out.txt good.txt | less
	rm out.txt good.txt

tar : 
	tar -cf fabe0940.tar README makefile scanner.l parser.y getopt.c token.h
