MAKEFLAGS += --no-print-directory

dependencies := src/toolchain/lexer.cpp src/toolchain/parser.cpp

build:
	@g++ -Wall -Wextra -std=c++17 src/kappa.cpp $(dependencies) -o bin/kappa

kappa: $(filter-out $@,$(MAKECMDGOALS))
	@./bin/kappa $<

test:
	@make kappa eg/main.K

clean:
	@del /q "bin\*"
