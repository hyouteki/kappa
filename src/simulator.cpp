#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <optional>
#include <cassert>
#include "parser.hpp"
#include "simulator.hpp"
#include "builtins.hpp"

std::unordered_map<std::string, Fun> funs;
Var_Map global_vars;

Expr reduce_to_basic_expr(const Expr expr, const Expr_Kind kind, Var_Map* vars) {
    return fun_call_to_expr(mix_to_expr(expr, kind, vars), kind, vars);
}

Expr eval(const Lexeme_Kind op, const Expr expr1, const Expr expr2) {
    switch (expr1.kind) {
        case INT: {
            switch (op) {
                case PLUS: return *(new Expr(expr1.int_val()+expr2.int_val()));
                default:
                    std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
                    std::cerr << "ERROR: Invalid operator '" << lexeme_kind_to_str(op);
                    std::cerr << "' for datatype '" << expr_kind_to_str(expr1.kind) << "'" << std::endl;
                    exit(1);
            }
            default:
                std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
                std::cerr << "ERROR: Invalid expression "; expr1.print();
                std::cerr << " " << lexeme_kind_to_str(op) << " "; expr2.print();
                std::cerr << std::endl;
                exit(1);
        } break;
    }
    return {};
}

Expr mix_to_expr(const Expr expr, Expr_Kind kind, Var_Map* vars) {
    if (expr.kind != _MIX) return expr;
    assert(expr.val1 != nullptr);
    assert(expr.val2 != nullptr);
    Expr expr1 = fun_call_to_expr(*expr.val1, kind, vars);
    Expr expr2 = fun_call_to_expr(*expr.val2, kind, vars);
    if (expr1.kind != expr2.kind) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Type mismatch '" << expr_kind_to_str(expr1.kind);
        std::cerr << "' != '" << expr_kind_to_str(expr2.kind) << "'" << std::endl;
        exit(1);
    }
    if (expr1.kind != kind) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected type '" << expr_kind_to_str(kind);
        std::cerr << "', but got '" << expr_kind_to_str(expr1.kind) << "'" << std::endl;
        exit(1);
    }
    return eval(expr.op, expr1, expr2);
}

Expr fun_call_to_expr(const Expr expr, const Expr_Kind kind, Var_Map* vars) {
    if (expr.kind != FUN_CALL) {
        if (!are_expr_kinds_compatible(kind, expr.kind)) {
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
            std::cerr << "ERROR: Type mismatch, expected type '" << expr_kind_to_str(kind);
            std::cerr << "'; but got argument of type '";
            std::cerr << expr_kind_to_str(expr.kind) << "'" << std::endl;
            exit(1);
        } else return expr;
    }
    Fun fun = get_fun(expr.fun_call());
    if (!are_expr_kinds_compatible(kind, fun.return_type)) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Type mismatch, expected type '" << expr_kind_to_str(kind);
        std::cerr << "'; but function return type is '";
        std::cerr << expr_kind_to_str(fun.return_type) << "'" << std::endl;
        exit(1);
    }
    return *eval_fun_call(fun, expr.fun_call(), vars);
}

Fun get_fun(const Fun_Call fun_call) {
    if (funs.find(fun_call.name) == funs.end()) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":";
        std::cerr << __LINE__ << std::endl;
        std::cerr << "ERROR: Cannot find the function '";
        std::cerr << fun_call.name << "'" << std::endl;
        exit(1);
    }
    return funs.at(fun_call.name);
}

Fun_Call replace_vars_to_exprs(const Fun_Call fun_call, Var_Map* vars) {
    Fun_Call* tmp = new Fun_Call();
    tmp->name = fun_call.name;
    for (size_t i = 0; i < fun_call.args.size(); ++i)
        tmp->args.push_back(*var_expr_to_expr(fun_call.args[i], vars));
    return *tmp;
}

std::optional<Expr> var_expr_to_expr(const Expr expr, Var_Map* vars) {
    if (expr.kind != _VAR) return expr;
    if (vars->find(expr.str_val()) != vars->end())
        return vars->at(expr.str_val()).expr;
    if (global_vars.find(expr.str_val()) != global_vars.end())
        return global_vars.at(expr.str_val()).expr;
    std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
    std::cerr << "ERROR: Undefined variable '" << expr.str_val() << "'" << std::endl;
    exit(1);
}

std::optional<Expr> eval_fun_call(
    const Fun fun,
    const Fun_Call fun_call,
    Var_Map* vars
) {
    size_t i = 0;
    Var_Map fun_call_vars;
    for (; i < fun_call.args.size() && i < fun.args.size(); ++i) {
        std::optional<Expr> out = var_expr_to_expr(fun_call.args[i], vars);
        if (!out) {
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
            std::cerr << "ERROR: Undefined variable '" << fun_call.args[i].str_val();
            std::cerr << "'" << std::endl;
            exit(1);
        }
        Expr expr = *out;
        if (expr.kind == FUN_CALL) expr = *eval_fun_call(
            get_fun(expr.fun_call()), expr.fun_call(), vars);
        if (expr.kind != fun.args[i].type) {
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
            std::cerr << "ERROR: Expected type '" << expr_kind_to_str(fun.args[i].type);
            std::cerr << "' for variable '" << fun.args[i].name << "'; but got argument of type '";
            std::cerr << expr_kind_to_str(expr.kind) << "'" << std::endl;
            exit(1);
        }
        Var* var = new Var(fun.args[i]);
        var->expr = expr;
        fun_call_vars[var->name] = *var;
    }
    if (i > fun.args.size()) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Signature of function call does not match the";
        std::cerr << " expected signature" << std::endl;
        exit(1);
    }
    if (i < fun.args.size()) {
        if (fun.args[i].expr) fun_call_vars[fun.args[i].name] = fun.args[i];
        else {
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
            std::cerr << "ERROR: Signature of function call does not match the";
            std::cerr << " expected signature" << std::endl;
            exit(1);
        }
    }
    std::optional<Expr> out = simul(fun.block, &fun_call_vars, true);
    return (out && out->kind == FUN_CALL)? simul_fun_call(out->fun_call(), &fun_call_vars): out;
}

std::optional<Expr> simul_fun_call(const Fun_Call fun_call, Var_Map* vars) {
    Fun_Call tmp = replace_vars_to_exprs(fun_call, vars);
    if (!mapper(tmp)) return eval_fun_call(get_fun(fun_call), tmp, vars);
    return {};
}

std::optional<Expr> simul_stmt(
    const Stmt stmt,
    Var_Map* vars,
    const bool can_return,
    const bool is_global
) {
    switch (stmt.kind) {
        case Stmt::FUN_DEF: {
            if (funs.find(stmt.fun().name) != funs.end()) {
                std::cerr << __FILE__ << ":" << __FUNCTION__ << ":";
                std::cerr << __LINE__ << std::endl;
                std::cerr << "ERROR: Redefination of function with signature '";
                std::cerr << stmt.fun().name << std::endl;
                exit(1);
            }
            funs[stmt.fun().name] = stmt.fun();
        } break;
        case Stmt::IF: {
            Stmt* condition = stmt.if_cond().condition;
            if (condition->kind != Stmt::EXPR) {
                std::cerr << __FILE__ << ":" << __FUNCTION__ << ":";
                std::cerr << __LINE__ << std::endl;
                std::cerr << "ERROR: Invalid condition" << std::endl;
                exit(1);
            }
            Expr expr = condition->val.expr;
            switch (expr.kind) {
                case BOOL:
                    if (expr.bool_val()) simul(stmt.if_cond().then_block, vars);
                    else simul(stmt.if_cond().else_block, vars);
                    break;
                case FUN_CALL: {
                    std::optional<Expr> out = simul_fun_call(expr.fun_call(), vars);
                    if (out) {
                        expr = *out;
                        switch (expr.kind) {
                            case BOOL:
                                if (expr.bool_val()) simul(stmt.if_cond().then_block, vars);
                                else simul(stmt.if_cond().else_block, vars);
                                break;
                            default:
                                simul(stmt.if_cond().then_block, vars);
                                exit(1);
                        }
                    } else simul(stmt.if_cond().else_block, vars);
                } break;
                default:
                    simul(stmt.if_cond().then_block, vars);
            }
        } break;
        case Stmt::VAR: {
            Var var = stmt.var();
            if (var.type == _RE_ASS) {
                if (is_global) {
                    if (global_vars.find(var.name) == global_vars.end()) {
                        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
                        std::cerr << "ERROR: Variable '" << var.name;
                        std::cerr << "' is not declared" << std::endl;
                        exit(1);
                    }
                    if (!global_vars.at(var.name).mut) {
                        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
                        std::cerr << "ERROR: Illegal operation, variable '" << var.name;
                        std::cerr << "' is not mutable" << std::endl;
                        exit(1);
                    }
                    Expr expr = reduce_to_basic_expr(*var.expr, global_vars.at(var.name).type, vars);
                    if (!are_expr_kinds_compatible(global_vars.at(var.name).type, expr.kind)) {
                        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
                        std::cerr << "ERROR: Type mismatch, expected type '";
                        std::cerr << expr_kind_to_str(global_vars.at(var.name).type);
                        std::cerr << "'; got '" << expr_kind_to_str(expr.kind) << "'" << std::endl;
                        exit(1);
                    }
                    global_vars[var.name].expr = expr;
                } else {
                    if (vars->find(var.name) == vars->end()) {
                        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
                        std::cerr << "ERROR: Variable '" << var.name;
                        std::cerr << "' is not declared" << std::endl;
                        exit(1);
                    }
                    if (!vars->at(var.name).mut) {
                        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
                        std::cerr << "ERROR: Illegal operation, variable '" << var.name;
                        std::cerr << "' is not mutable" << std::endl;
                        exit(1);
                    }
                    Expr expr = reduce_to_basic_expr(*var.expr, vars->at(var.name).type, vars);
                    if (!are_expr_kinds_compatible(vars->at(var.name).type, expr.kind)) {
                        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
                        std::cerr << "ERROR: Type mismatch, expected type '";
                        std::cerr << expr_kind_to_str(vars->at(var.name).type);
                        std::cerr << "'; got '" << expr_kind_to_str(expr.kind) << "'" << std::endl;
                        exit(1);
                    }
                    var.mut = true;
                    var.type = vars->at(var.name).type;
                    var.expr = expr;
                    vars->find(var.name)->second = var;
                }
                return {};
            }
            Expr expr = reduce_to_basic_expr(*var.expr, var.type, vars);
            std::cout << "hello "; expr.print(); std::cout << std::endl;
            if (is_global) {
                if (global_vars.find(var.name) != global_vars.end()) {
                    if (!global_vars[var.name].mut) {
                        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
                        std::cerr << "ERROR: Illegal operation, variable '" << var.name;
                        std::cerr << "' is not mutable" << std::endl;
                        exit(1);
                    }
                    if (!are_expr_kinds_compatible(global_vars.at(var.name).type, expr.kind)) {
                        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
                        std::cerr << "ERROR: Type mismatch, expected type '";
                        std::cerr << expr_kind_to_str(global_vars.at(var.name).type);
                        std::cerr << "'; got '" << expr_kind_to_str(expr.kind) << "'" << std::endl;
                        exit(1);
                    }
                }
                global_vars.find(var.name)->second.expr = expr;
            } else {
                if (vars->find(var.name) != vars->end()) {
                    if (!vars->at(var.name).mut) {
                        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
                        std::cerr << ": ERROR: Illegal operation, variable '" << var.name;
                        std::cerr << "' is not mutable" << std::endl;
                        exit(1);
                    }
                    if (!are_expr_kinds_compatible(vars->at(var.name).type, expr.kind)) {
                        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
                        std::cerr << "ERROR: Type mismatch, expected type '";
                        std::cerr << expr_kind_to_str(vars->at(var.name).type);
                        std::cerr << "'; got '" << expr_kind_to_str(expr.kind) << "'" << std::endl;
                        exit(1);
                    }
                }
                vars->find(var.name)->second.expr = expr;
            }
            std::cout << "hello "; expr.print(); std::cout << std::endl;
        } break;
        case Stmt::EXPR: {
            if (stmt.expr().kind == FUN_CALL)
                simul_fun_call(stmt.expr().fun_call(), vars);
        } break;
        case Stmt::RETURN: {
            if (!can_return) {
                std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
                std::cerr << "ERROR: Illegal Stmt_Kind; cannot return outside fo a function" << std::endl;
                exit(1);
            }
            return stmt.expr();
        } break;
        default:
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
            std::cerr << "ERROR: Invalid Stmt_Kind" << std::endl;
            exit(1);
    }
    return {};
}

std::optional<Expr> simul(
    const std::vector<Stmt> stmts,
    Var_Map* vars,
    const bool can_return,
    const bool is_global
) {
    for (Stmt stmt: stmts) {
        std::optional<Expr> out = simul_stmt(stmt, vars, can_return, is_global);
        if (out) return *out;
    }
    return {};
}
