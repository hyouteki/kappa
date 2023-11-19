#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "parser.hpp"
#include "simulator.hpp"
#include "builtins.hpp"

std::unordered_map<std::string, Fun> sign_fun_map;

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

void simul_stmt(const Stmt stmt) {
    switch (stmt.kind) {
        case Stmt::FUN_DEF: {
            if (sign_fun_map.find(stmt.val.fun.name) != sign_fun_map.end()) {
                std::cerr << __FILE__ << ":" << __FUNCTION__ << ":";
                std::cerr << __LINE__ << std::endl;
                std::cerr << "ERROR: Redefination of function with signature '";
                std::cerr << stmt.val.fun.name << std::endl;
                exit(1);
            }
            sign_fun_map[stmt.val.fun.name] = stmt.val.fun;
        } break;
        case Stmt::IF: {
            If if_cond = stmt.val.if_cond;
            Stmt* condition = if_cond.condition;
            if (condition->kind != Stmt::EXPR) {
                std::cerr << __FILE__ << ":" << __FUNCTION__ << ":";
                std::cerr << __LINE__ << std::endl;
                std::cerr << "ERROR: Invalid condition" << std::endl;
                exit(1);
            }
            Expr expr = condition->val.expr;
            switch (expr.kind) {
                case Expr::BOOL:
                    if (expr.val.bool_val) simul(if_cond.then_block);
                    else simul(if_cond.else_block);
                    break;
                case Expr::FUN_CALL: {
                    std::optional<Expr> out = simul_fun_call(expr.val.fun_call);
                    if (out) {
                        expr = *out;
                        switch (expr.kind) {
                            case Expr::BOOL:
                                if (expr.val.bool_val) simul(if_cond.then_block);
                                else simul(if_cond.else_block);
                                break;
                            default:
                                simul(if_cond.then_block);
                                exit(1);
                        }
                    } else simul(if_cond.else_block);
                } break;
                default:
                    simul(if_cond.then_block);
            }
        } break;
        case Stmt::EXPR: {
            if (stmt.val.expr.kind == Expr::FUN_CALL)
                simul_fun_call(stmt.val.expr.val.fun_call);
        } break;
        default:
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
            std::cerr << "ERROR: Invalid Stmt_Kind" << std::endl;
            exit(1);
    }
}

void simul(const std::vector<Stmt> stmts) {
    for (Stmt stmt: stmts) simul_stmt(stmt);
}
