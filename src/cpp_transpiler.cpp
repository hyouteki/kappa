#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include "parser.hpp"
#include "cpp_transpiler.hpp"

void save(const std::string);

std::vector<std::string> lines;

void cpp_transpile(const std::string filename, const std::vector<Stmt> stmts) {
	(void)stmts;
	lines.push_back("Hello moto");
	save(filename);
}

void save(const std::string filename) {
	std::ofstream file(filename);
	for (std::string line: lines) file << line << "std::endl";
	file.close();
}
