#ifndef KAPPA_PRINT_PARSER_CPP_
#define KAPPA_PRINT_PARSER_CPP_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include "parser.hpp"

void print_parser(std::vector<Stmt>);

void print_parser(std::vector<Stmt> stmts) {
	std::cout << "[PRINT_PARSER]" << std::endl;
	for (Stmt stmt: stmts) {
		stmt.print();
		std::cout << std::endl;
	}
}

#endif
