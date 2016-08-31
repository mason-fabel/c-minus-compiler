CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -O2
# LFLAGS :=
PROG := c-
SRC := $(wildcard *.c)
OBJ := $(patsubst %.c, %.o, $(SRC))

.PHONY : clean rebuild

$(PROG) : $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean :
	rm -f $(PROG) $(OBJ)

rebuild :
	make clean
	make $(PROG)
