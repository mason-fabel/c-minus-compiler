SRC := $(wildcard src/*.cpp)
GEN := src/scanner.cpp src/parser.cpp src/parser.h src/parser.output
OBJ := $(addprefix obj/,$(notdir $(SRC:.cpp=.o))) obj/scanner.o obj/parser.o
BIN := c-

BFLAGS := --verbose --report=all -Wall -Werror
CFLAGS := -Wall -Wextra -DYYDEBUG
LFLAGS := $(CFLAGS)

.PHONY : clean test

$(BIN) : $(OBJ)
	g++ $(LFLAGS) -o $@ $^

$(OBJ) : $(GEN)

src/scanner.cpp : src/scanner.l src/parser.h
	flex --outfile=src/scanner.cpp src/scanner.l

src/parser.cpp src/parser.h : src/parser.y
	bison $(BFLAGS) --defines=src/parser.h --output=src/parser.cpp src/parser.y

obj/%.o : src/%.cpp $(GEN)
	g++ $(CFLAGS) -c -o $@ $<

clean : 
	rm -rf $(GEN)
	rm -rf $(OBJ)
	rm -rf $(BIN)

test : $(BIN)
	./$(BIN) -P test/tiny.c- > test.out
	diff -y test.out test/tiny.out | less
	rm -f test.out

tar : test clean
	tar -cf fabe0940.tar makefile src obj
