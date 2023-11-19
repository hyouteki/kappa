MAKEFLAGS += --no-print-directory

CC = g++
CFLAGS = -Wall -Wextra -std=c++17
dependencies := src/lexer.cpp src/parser.cpp src/simulator.cpp src/builtins.cpp

build:
	@$(CC) $(CFLAGS) src/kappa.cpp $(dependencies) -o bin/kappa

test:
	@./bin/kappa eg/main.K
