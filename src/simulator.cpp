#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <optional>
#include "parser.hpp"
#include "simulator.hpp"
#include "builtins.hpp"

std::unordered_map<std::string, Fun> funs;
std::unordered_map<std::string, Var> global_vars;

Fun_Call replace_vars_to_exprs(
    const Fun_Call fun_call,
    std::unordered_map<std::string, Var>* vars
) {
    Fun_Call* tmp = new Fun_Call();
    tmp->name = fun_call.name;
    for (size_t i = 0; i < fun_call.args.size(); ++i)
        tmp->args.push_back(*var_expr_to_expr(fun_call.args[i], vars));
    return *tmp;
}

std::optional<Expr> var_expr_to_expr(
    const Expr expr,
    std::unordered_map<std::string, Var>* vars
) {
    if (expr.kind != VAR) return expr;
    if (vars->find(expr.str_val()) != vars->end())
        return vars->at(expr.str_val()).expr;
    if (global_vars.find(expr.str_val()) != global_vars.end())
        return global_vars.at(expr.str_val()).expr;
    return {};
}

std::optional<Expr> eval_fun_call(
    const Fun fun,
    const Fun_Call fun_call,
    std::unordered_map<std::string, Var>* vars
) {
    size_t i = 0;
    std::unordered_map<std::string, Var> fun_call_vars;
    for (; i < fun_call.args.size() && i < fun.args.size(); ++i) {
        std::optional<Expr> out = var_expr_to_expr(fun_call.args[i], vars);
        if (!out) {
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
            std::cerr << "ERROR: Undefined variable '" << fun_call.args[i].str_val();
            std::cerr << "'" << std::endl;
            exit(1);
        }
        Expr expr = *out;
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

std::optional<Expr> simul_fun_call(
    const Fun_Call fun_call,
    std::unordered_map<std::string, Var>* vars
) {
    Fun_Call tmp = replace_vars_to_exprs(fun_call, vars);
    if (!mapper(tmp)) {
        if (funs.find(tmp.name) == funs.end()) {
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":";
            std::cerr << __LINE__ << std::endl;
            std::cerr << "ERROR: Cannot find the function '";
            std::cerr << tmp.name << "'" << std::endl;
            exit(1);
        } else return eval_fun_call(funs.at(tmp.name), tmp, vars);
    }
    return {};
}

std::optional<Expr> simul_stmt(
    const Stmt stmt,
    std::unordered_map<std::string, Var>* vars,
    const bool can_return
) {
    (void)vars;
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
            if (global_vars.find(var.name) != global_vars.end()) {
                Var tmp = global_vars.at(var.name);
                if (tmp.type != var.type) {
                    std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
                    std::cerr << ": ERROR: Type mismatch, expected type '" << expr_kind_to_str(tmp.type);
                    std::cerr << "'; got '" << expr_kind_to_str(var.type) << "'" << std::endl;
                    exit(1);
                }
            }
            global_vars[var.name] = var;
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
    std::unordered_map<std::string, Var>* vars,
    const bool can_return
) {
    for (Stmt stmt: stmts) {
        std::optional<Expr> out = simul_stmt(stmt, vars, can_return);
        if (out) return *out;
    }
    return {};
}
