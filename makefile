SRC := $(wildcard src/*.cpp)
GEN := src/scanner.cpp src/parser.cpp src/parser.h src/parser.output
OBJ := $(addprefix obj/,$(notdir $(SRC:.cpp=.o))) obj/scanner.o obj/parser.o
BIN := c-

BFLAGS := --verbose --report=all -Wall -Werror
CFLAGS := -Wall -Wextra -Wno-switch -DYYDEBUG
LFLAGS := $(CFLAGS)

.PHONY : clean test $(TESTS)

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

tar : test clean
	tar -cf fabe0940.tar makefile src obj

TESTS := test-basic test-everything test-exp test-exp2 test-init test-record test-scope test-small test-tiny test-tinybad test-tinygood test-tinyscope test-tinytype test-whileif test-z

test : test-regression

test-regression : test-tiny test-small test-whileif test-z test-tinyscope test-tinybad test-tinygood test-everything test-basic

test-all : $(TESTS)

test-basic : $(BIN)
	./$(BIN) -P test/basicAll4.c- | diff --width=190 -y - test/basicAll4.out | less

test-everything : $(BIN)
	./$(BIN) -P test/everything05.c- | diff --width=190 -y - test/everything05.out | less

test-exp : $(BIN)
	./$(BIN) -P test/exp.c- | diff --width=190 -y - test/exp.out | less

test-exp2 : $(BIN)
	./$(BIN) -P test/exp2.c- | diff --width=190 -y - test/exp2.out | less

test-init : $(BIN)
	./$(BIN) -P test/init.c- | diff --width=190 -y - test/init.out | less

test-record : $(BIN)
	./$(BIN) -P test/record.c- | diff --width=190 -y - test/record.out | less

test-scope : $(BIN)
	./$(BIN) -P test/scope.c- | diff --width=190 -y - test/scope.out | less

test-small : $(BIN)
	./$(BIN) -P test/small.c- | diff --width=190 -y - test/small.out | less

test-tiny : $(BIN)
	./$(BIN) -P test/tiny.c- | diff --width=190 -y - test/tiny.out | less

test-tinybad : $(BIN)
	./$(BIN) -P test/tinybad.c- | diff --width=190 -y - test/tinybad.out | less

test-tinygood : $(BIN)
	./$(BIN) -P test/tinygood.c- | diff --width=190 -y - test/tinygood.out | less

test-tinyscope : $(BIN)
	./$(BIN) -P test/tinyscope.c- | diff --width=190 -y - test/tinyscope.out | less

test-tinytype : $(BIN)
	./$(BIN) -P test/tinyType.c- | diff --width=190 -y - test/tinyType.out | less

test-whileif : $(BIN)
	./$(BIN) -P test/whileif.c- | diff --width=190 -y - test/whileif.out | less

test-z : $(BIN)
	./$(BIN) -P test/z.c- | diff --width=190 -y - test/z.out | less
