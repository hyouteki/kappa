MAKEFLAGS += --no-print-directory

CC = g++
CFLAGS = -Wall -Wextra -std=c++17
dependencies := src/toolchain/lexer.cpp src/toolchain/parser.cpp

build:
	@$(CC) $(CFLAGS) src/kappa.cpp $(dependencies) -o bin/kappa

kappa: $(filter-out $@,$(MAKECMDGOALS))
	@./bin/kappa $<

test:
	@make kappa eg/main.K

clean:
	@del /q "bin\*"
