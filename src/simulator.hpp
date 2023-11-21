#ifndef KAPPA_SIMULATOR_HPP_
#define KAPPA_SIMULATOR_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <optional>
#include "parser.hpp"

Expr fun_call_to_expr(
    const Expr,
    const Expr_Kind,
    std::unordered_map<std::string, Var>*
);
Fun get_fun(const Fun_Call);
Fun_Call replace_vars_to_exprs(
    const Fun_Call,
    std::unordered_map<std::string, Var>*
);
std::optional<Expr> var_expr_to_expr(
    const Expr,
    std::unordered_map<std::string, Var>*
);
std::optional<Expr> eval_fun_call(
    const Fun,
    const Fun_Call,
    std::unordered_map<std::string, Var>*
);
std::optional<Expr> simul_fun_call(
    const Fun_Call,
    std::unordered_map<std::string, Var>*
);
std::optional<Expr> simul_stmt(
    const Stmt,
    std::unordered_map<std::string, Var>*,
    const bool = false,
    const bool = false
);
std::optional<Expr> simul(
    const std::vector<Stmt>,
    std::unordered_map<std::string, Var>*,
    const bool = false,
    const bool = false
);

#endif // KAPPA_SIMULATOR_HPP_
