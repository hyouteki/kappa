#ifndef KAPPA_C_TRANSPILER_HPP_
#define KAPPA_C_TRANSPILER_HPP_

#include <stdio.h>
#include <stdlib.h>
#include "parser.hpp"

void write_header(const std::string filename);
void transpile(const std::string filename, const std::vector<Stmt>);
void exit_();


#endif
