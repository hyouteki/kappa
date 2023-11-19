#ifndef KAPPA_SIMULATOR_HPP_
#define KAPPA_SIMULATOR_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <unordered_map>
#include "parser.hpp"

void simul_stmt(const Stmt);
void simul(const std::vector<Stmt>);

#endif // KAPPA_SIMULATOR_HPP_
