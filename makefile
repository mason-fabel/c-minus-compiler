SHELL := /bin/bash

SRC := $(wildcard src/*.cpp) $(wildcard src/analysis/*.cpp)
GEN := src/scanner.cpp src/parser.cpp src/parser.h src/parser.output
OBJ := $(addprefix obj/,$(notdir $(SRC:.cpp=.o))) obj/scanner.o obj/parser.o
BIN := c-

BFLAGS := --verbose --report=all -Wall
CFLAGS := -g -Wall -Wextra -Wno-switch -DYYDEBUG
LFLAGS := -Wall -Wextra

.PHONY : clean submit

$(BIN) : $(OBJ)
	g++ $(LFLAGS) -o $@ $^

$(OBJ) : $(GEN)

src/scanner.cpp : src/scanner.l src/parser.h
	flex --outfile=src/scanner.cpp src/scanner.l

src/parser.cpp src/parser.h : src/parser.y
	bison $(BFLAGS) --defines=src/parser.h --output=src/parser.cpp src/parser.y

obj/%.o : src/%.cpp $(GEN)
	g++ $(CFLAGS) -c -o $@ $<

obj/%.o : src/analysis/%.cpp
	g++ $(CFLAGS) -c -o $@ $<

clean : 
	rm -rf $(GEN)
	rm -rf $(OBJ)
	rm -rf $(BIN)

rebuild : clean $(BIN)

tar : clean
	tar -cf fabe0940.tar makefile src obj

submit : tar
	curl -s -S -F student=fabel -F assignment="CS445 F16 Assignment 6" \
		-F "submittedfile=@fabe0940.tar" \
		"http://ec2-52-89-93-46.us-west-2.compute.amazonaws.com/cgi-bin/fileCapture.py"

test : test-all

test-all : test-arrays test-arrays2 test-basic-all test-basic-extra test-battleship test-bullsandcows test-call test-call4 test-errorloc test-errormessages test-err test-everything test-exp test-exp2 test-factorial test-factorial2 test-factorialr test-factor test-fractal test-gcd test-gcdsm test-init test-mixed test-nano test-op test-overkill test-parmtiny test-poker test-redefined test-scope test-simple test-simple-define test-small test-sudoku test-tictactoe test-tiny test-tiny-bad test-tiny-good test-tiny-scope test-tiny-type test-undefined test-uninitialized test-unused test-walsh test-whileif

test-arrays : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/arrays.c-) test/arrays.out | less

test-arrays2 : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/arrays2.c-) test/arrays2.out | less

test-basic-all : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/basicAll4.c-) test/basicAll4.out | less

test-basic-extra : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/basicExtra.c-) test/basicExtra.out | less

test-battleship : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/battleship.c-) test/battleship.out | less

test-bullsandcows : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/bullsandcows.c-) test/bullsandcows.out | less

test-call : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/call.c-) test/call.out | less

test-call4 : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/call4.c-) test/call4.out | less

test-errorloc : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/errorloc.c-) test/errorloc.out | less

test-errormessages : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/errormessages.c-) test/errormessages.out | less

test-err : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/err.c-) test/err.out | less

test-everything : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/everything05.c-) test/everything05.out | less

test-exp : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/exp.c-) test/exp.out | less

test-exp2 : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/exp2.c-) test/exp2.out | less

test-factorial : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/factorial.c-) test/factorial.out | less

test-factorial2 : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/factorial2.c-) test/factorial2.out | less

test-factorialr : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/factorialr.c-) test/factorialr.out | less

test-factor : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/factor.c-) test/factor.out | less

test-fractal : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/fractal.c-) test/fractal.out | less

test-gcd : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/gcd.c-) test/gcd.out | less

test-gcdsm : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/gcdsm.c-) test/gcdsm.out | less

test-init : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/init.c-) test/init.out | less

test-mixed : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/mixed.c-) test/mixed.out | less

test-nano : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/nano.c-) test/nano.out | less

test-op : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/op.c-) test/op.out | less

test-overkill : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/overkill.c-) test/overkill.out | less

test-parmtiny : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/parmtiny.c-) test/parmtiny.out | less

test-poker : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/poker.c-) test/poker.out | less

test-redefined : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/redefined.c-) test/redefined.out | less

test-scope : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/scope.c-) test/scope.out | less

test-simple : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/simple.c-) test/simple.out | less

test-simple-define : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/simpleDefine.c-) test/simpleDefine.out | less

test-small : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/small.c-) test/small.out | less

test-sudoku : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/sudoku.c-) test/sudoku.out | less

test-tictactoe : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/tictactoe.c-) test/tictactoe.out | less

test-tiny : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/tiny.c-) test/tiny.out | less

test-tiny-bad : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/tinybad.c-) test/tinybad.out | less

test-tiny-good : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/tinygood.c-) test/tinygood.out | less

test-tiny-scope : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/tinyscope.c-) test/tinyscope.out | less

test-tiny-type : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/tinyType.c-) test/tinyType.out | less

test-undefined : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/undefined.c-) test/undefined.out | less

test-uninitialized : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/uninitialized.c-) test/uninitialized.out | less

test-unused : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/unused.c-) test/unused.out | less

test-walsh : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/walsh.c-) test/walsh.out | less

test-whileif : $(BIN)
	sdiff --width=190 <(echo $@; ./c- -P test/whileif.c-) test/whileif.out | less
