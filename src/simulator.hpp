#ifndef KAPPA_SIMULATOR_HPP_
#define KAPPA_SIMULATOR_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <optional>
#include "parser.hpp"

#define Var_Map std::unordered_map<std::string, Var>

bool eval_condition(const Expr, Var_Map*);
Expr reduce_to_basic_expr(const Expr, const Expr_Kind, Var_Map*, bool = true);
Expr eval(const Lexeme_Kind, const Expr, const Expr);
Expr mix_to_expr(const Expr, Expr_Kind, Var_Map*, bool = true);
Expr fun_call_to_expr(const Expr, const Expr_Kind, Var_Map*, bool = true);
Fun get_fun(const Fun_Call);
Fun_Call replace_vars_to_exprs(const Fun_Call, Var_Map*);
std::optional<Expr> var_expr_to_expr(const Expr, Var_Map*);
std::optional<Expr> eval_fun_call(const Fun, const Fun_Call, Var_Map*);
std::optional<Expr> simul_fun_call(const Fun_Call, Var_Map*);
std::optional<Expr> simul_stmt(
    const Stmt,
    Var_Map*,
    const bool = false,
    const bool = false
);
std::optional<Expr> simul(
    const std::vector<Stmt>,
    Var_Map*,
    const bool = false,
    const bool = false
);

#endif // KAPPA_SIMULATOR_HPP_
