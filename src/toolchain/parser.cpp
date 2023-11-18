#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"

void Expr::print() const {
    switch (this->kind) {
        case ANY_EXPR:
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

void Fun::print() const {
    std::cout << this->name << "(";
    for (size_t i = 0; i < this->args.size(); ++i) {
        this->args[i].print();
        if (i < this->args.size()-1) std::cout << ", ";
    }
    std::cout << ")";
}

Fun parse_fun_def(Lexer *lexer) {
    lexer->assert_lexeme_front("fun");
    lexer->del_front();
    lexer->assert_lexeme_front(NAME);
    Fun* fun = new Fun();
    fun->name = lexer->front().str;
    lexer->del_front();
    lexer->assert_lexeme_front(OPEN_PAREN);
    lexer->del_front();
    while (!lexer->front().equal(CLOSE_PAREN)) {
        lexer->assert_lexeme_front(NAME);
        fun->args.push_back((Expr){
            .kind = ANY_EXPR,
            .val = (Expr_Val){.str_val = lexer->front().str}
        });
        lexer->del_front();
        if (lexer->front().equal(CLOSE_PAREN)) continue;
        lexer->assert_lexeme_front(COMMA);
        lexer->del_front();
    }
    return *fun;
}
