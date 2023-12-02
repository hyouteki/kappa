MAKEFLAGS += --no-print-directory

CC = g++
CFLAGS = -Wall -Wextra -std=c++17

dependencies += src/lexer.cpp 
dependencies += src/parser.cpp 
dependencies += src/simulator.cpp 
dependencies += src/builtins.cpp
dependencies += src/cpp_transpiler.cpp

build:
	@$(CC) $(CFLAGS) src/kappa.cpp $(dependencies) -o bin/kappa

test:
	@./bin/kappa eg/main.K
