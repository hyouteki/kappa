#ifndef KAPPA_SIMULATOR_HPP_
#define KAPPA_SIMULATOR_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <unordered_map>
#include <optional>
#include "parser.hpp"

std::optional<Expr> simul_fun_call(const Fun_Call);
std::optional<Expr> simul_stmt(const Stmt, const bool = false);
std::optional<Expr> simul(const std::vector<Stmt>, const bool = false);

#endif // KAPPA_SIMULATOR_HPP_
