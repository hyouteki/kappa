#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "parser.hpp"
#include "simulator.hpp"
#include "builtins.hpp"

std::unordered_map<std::string, Fun> sign_fun_map;

void simul_stmt(const Stmt stmt) {
    switch (stmt.kind) {
        case Stmt::FUN_DEF: {
            if (sign_fun_map.find(stmt.val.fun.name) != sign_fun_map.end()) {
                std::cerr << "ERROR: Redefination of function with signature '";
                std::cerr << stmt.val.fun.name << std::endl;
                exit(1);
            }
            sign_fun_map[stmt.val.fun.name] = stmt.val.fun;
        } break;
        case Stmt::EXPR: {
            if (stmt.val.expr.kind == Expr::FUN_CALL) {
                if (!mapper(stmt.val.expr.val.fun_call)) {
                    if (sign_fun_map.find(stmt.val.expr.val.fun_call.name) == sign_fun_map.end()) {
                        std::cerr << "ERROR: Cannot find the function '";
                        std::cerr << stmt.val.expr.val.fun_call.name << "'" << std::endl;
                        exit(1);
                    } else {
                        Fun fun = sign_fun_map.at(stmt.val.expr.val.fun_call.name);
                        fun.execute(stmt.val.expr.val.fun_call);
                    }
                }
            }
        } break;
        default:
            std::cerr << "Invalid Stmt_Kind" << std::endl;
            exit(1);
    }
}

void simul(const std::vector<Stmt> stmts) {
    for (Stmt stmt: stmts) simul_stmt(stmt);
}
