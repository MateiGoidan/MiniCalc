CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -lX11 -lm
SRC = $(wildcard src/*.c)
OUT = ./bin/MiniCalc

run: $(SRC)
	mkdir -p bin
	$(CC)  $(SRC) -o $(OUT) $(CFLAGS)
	$(OUT)

clean: 
	rm -rf ./bin/
