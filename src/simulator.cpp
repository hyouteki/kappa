#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <optional>
#include "parser.hpp"
#include "simulator.hpp"
#include "builtins.hpp"

std::unordered_map<std::string, Fun> sign_fun_map;
std::unordered_map<std::string, Var> global_vars;

std::optional<Expr> simul_fun_call(const Fun_Call fun_call) {
    if (!mapper(fun_call)) {
        if (sign_fun_map.find(fun_call.name) == sign_fun_map.end()) {
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":";
            std::cerr << __LINE__ << std::endl;
            std::cerr << "ERROR: Cannot find the function '";
            std::cerr << fun_call.name << "'" << std::endl;
            exit(1);
        } else return sign_fun_map.at(fun_call.name).eval(fun_call);
    }
    return {};
}

std::optional<Expr> simul_stmt(const Stmt stmt, const bool can_return) {
    switch (stmt.kind) {
        case Stmt::FUN_DEF: {
            if (sign_fun_map.find(stmt.fun().name) != sign_fun_map.end()) {
                std::cerr << __FILE__ << ":" << __FUNCTION__ << ":";
                std::cerr << __LINE__ << std::endl;
                std::cerr << "ERROR: Redefination of function with signature '";
                std::cerr << stmt.fun().name << std::endl;
                exit(1);
            }
            sign_fun_map[stmt.fun().name] = stmt.fun();
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
                    if (expr.bool_val()) simul(stmt.if_cond().then_block);
                    else simul(stmt.if_cond().else_block);
                    break;
                case FUN_CALL: {
                    std::optional<Expr> out = simul_fun_call(expr.fun_call());
                    if (out) {
                        expr = *out;
                        switch (expr.kind) {
                            case BOOL:
                                if (expr.bool_val()) simul(stmt.if_cond().then_block);
                                else simul(stmt.if_cond().else_block);
                                break;
                            default:
                                simul(stmt.if_cond().then_block);
                                exit(1);
                        }
                    } else simul(stmt.if_cond().else_block);
                } break;
                default:
                    simul(stmt.if_cond().then_block);
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
                simul_fun_call(stmt.expr().fun_call());
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

std::optional<Expr> simul(const std::vector<Stmt> stmts, bool can_return) {
    for (Stmt stmt: stmts) {
        std::optional<Expr> out = simul_stmt(stmt, can_return);
        if (out) return *out;
    }
    return {};
}
