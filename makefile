CC := g++
CFLAGS := -Wall -Wextra -DYYDEBUG
BIN := c-
GEN := scanner.c parser.c parser.h parser.output
OBJ := ast.o getopt.o parser.o scanner.o symtab.o

.PHONY : clean test

$(BIN) : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

scanner.c : scanner.l parser.h symtab.h token.h
	flex --outfile=scanner.c scanner.l

parser.c parser.h : parser.y ast.h token.h getopt.h symtab.h
	bison --verbose --report=all --defines=parser.h -Wall -Werror --output=parser.c parser.y

%.o : $.c
	$(CC) $(CFLAGS) -c $<

clean :
	rm -f $(BIN) $(OBJ) $(GEN)

test : $(BIN)
	./c- test/scannerTest.c- > out.txt
	cat test/scannerTest.out > good.txt
	diff --text --side-by-side out.txt good.txt | less
	rm out.txt good.txt

tar : 
	tar -cf fabe0940.tar README makefile \
		scanner.l parser.y \
		ast.c getopt.c symtab.c \
		ast.h getopt.h symtab.h token.h
