CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -O2
PROG := c-
SRC := main.c parser.tab.c lex.yy.c
GEN := lex.yy.c parser.tab.h parser.tab.c
OBJ := main.o parser.tab.o lex.yy.o

.PHONY : clean rebuild

$(PROG) : $(GEN) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

$(GEN) : parser.y scanner.l
	bison -d parser.y
	flex scanner.l

%.o : $.c
	$(CC) $(CFLAGS) -c $<

clean :
	rm -f $(PROG) $(OBJ) $(GEN)

rebuild :
	make clean
	make $(PROG)
