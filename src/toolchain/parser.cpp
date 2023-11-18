#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"

void Expr::print() const {
    switch (this->kind) {
        case VAR_EXPR:
        case STR_EXPR:
            std::cout << this->val.str_val;
            break;
        case FUN_CALL:
            this->val.fun_call.print();
            break;
        default:
            std::cerr << "Invalid Expr_Kind" <<std::endl;
    }
}

void Fun_Call::print() const {
    std::cout << this->name << "(";
    for (size_t i = 0; i < this->args.size(); ++i) {
        this->args[i].print();
        if (i < this->args.size()-1) std::cout << ", ";
    }
    std::cout << ")";
}

Expr parse_fun_call(Lexer *lexer) {
    lexer->assert_lexeme_front("fun");
    lexer->del_front();
    lexer->assert_lexeme_front(NAME);
    Fun_Call fun_call;
    fun_call.name = lexer->front().str;
    lexer->del_front();
    lexer->assert_lexeme_front(OPEN_PAREN);
    lexer->del_front();
    while (!lexer->front().equal(CLOSE_PAREN)) {
        lexer->assert_lexeme_front(NAME);
        fun_call.args.push_back((Expr){
            .kind = VAR_EXPR,
            .val = (Expr_Val){.str_val = lexer->front().str}
        });
        lexer->del_front();
        if (lexer->front().equal(CLOSE_PAREN)) continue;
        lexer->assert_lexeme_front(COMMA);
        lexer->del_front();
    }
    Expr* expr = new Expr();
    expr->kind = FUN_CALL;
    expr->val.fun_call.name = fun_call.name;
    expr->val.fun_call.args = fun_call.args;
    return *expr;
}
