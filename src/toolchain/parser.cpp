#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>
#include "lexer.hpp"
#include "parser.hpp"

std::unordered_set<std::string> reserved_strs = {"fun"};

void Expr::print() const {
    switch (this->kind) {
        case Expr::ANY:
        case Expr::STR:
            std::cout << this->val.str_val;
            break;
        case Expr::FUN_CALL:
            this->val.fun_call.print();
            break;
        default:
            std::cerr << "Invalid Expr_Kind" <<std::endl;
    }
}

Stmt::Stmt(const Fun fun) {
    this->kind = FUN_DEF;
    this->val.fun = fun;
}

Stmt::Stmt(const Expr expr) {
    this->kind = EXPR;
    this->val.expr = expr;
}

Expr iter_args(Lexer* lexer) {
    Expr* expr = new Expr();
    if (is_fun_call(lexer)) {
        Fun_Call fun_call = Fun_Call(lexer);
        expr->kind = Expr::FUN_CALL;
        expr->val.fun_call = fun_call;
        return *expr;
    } else if (lexer->front().equal(STR_LIT)) {
        expr->kind = Expr::STR;
        expr->val.str_val = lexer->front().str.substr(1,
            lexer->front().str.size()-2);
        lexer->del_front();
        return *expr;
    } else {
        std::cerr << lexer->filename << ":"; lexer->front().loc.print();
        std::cerr << ": ERROR: Invalid argument '" << lexer->front().str << "'" << std::endl;
        exit(1);
    }
}

Fun_Call::Fun_Call() {}

Fun_Call::Fun_Call(Lexer* lexer) {
    this->name = lexer->front().str;
    lexer->del_front();
    lexer->del_front();
    while (!lexer->front().equal(CLOSE_PAREN)) {
        this->args.push_back(iter_args(lexer));
        if (lexer->front().equal(CLOSE_PAREN)) continue;
        lexer->assert_lexeme_front(COMMA);
        lexer->del_front();
    }
    lexer->del_front();
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
    std::cout << "fun " << this->name << "(";
    for (size_t i = 0; i < this->args.size(); ++i) {
        this->args[i].print();
        if (i < this->args.size()-1) std::cout << ", ";
    }
    std::cout << ") {}";
}

bool is_fun_call(const Lexer* lexer) {
    std::vector<Lexeme_Kind> regex = {NAME, OPEN_PAREN};
    return (lexer->is_lexeme_front(regex) &&
        reserved_strs.find(lexer->front().str) == reserved_strs.end());
}

std::optional<Stmt> iter_lexer(Lexer* lexer) {
    Stmt* stmt;
    if (lexer->front().equal(SEMI)) {
        lexer->del_front();
        return {};
    } else if (lexer->front().equal("fun")) {
        stmt = new Stmt(Fun(lexer));
    } else if (is_fun_call(lexer)) {
        stmt = new Stmt((Expr){.kind = Expr::FUN_CALL,
            .val = (Expr::Expr_Val){.fun_call = Fun_Call(lexer)}});
    } else {
        std::cerr << lexer->filename << ":"; lexer->front().loc.print();
        std::cerr << ": ERROR: Invalid token '" << lexer->front().str << "'" << std::endl;
        exit(1);
    }
    return *stmt;
}

Fun::Fun(Lexer *lexer) {
    lexer->assert_lexeme_front("fun");
    lexer->del_front();
    lexer->assert_lexeme_front(NAME);
    this->name = lexer->front().str;
    lexer->del_front();
    lexer->assert_lexeme_front(OPEN_PAREN);
    lexer->del_front();
    while (!lexer->front().equal(CLOSE_PAREN)) {
        lexer->assert_lexeme_front(NAME);
        this->args.push_back((Expr){
            .kind = Expr::ANY,
            .val = (Expr::Expr_Val){.str_val = lexer->front().str}
        });
        lexer->del_front();
        if (lexer->front().equal(CLOSE_PAREN)) continue;
        lexer->assert_lexeme_front(COMMA);
        lexer->del_front();
    }
    lexer->del_front();
    lexer->assert_lexeme_front(OPEN_CURLY);
    lexer->del_front();
    while (!lexer->front().equal(CLOSE_CURLY)) {
        std::optional<Stmt> out = iter_lexer(lexer);
        if (out) this->block.push_back(*out);
    }
    lexer->del_front();
}

std::vector<Stmt> parse_lexer(Lexer *lexer) {
    std::vector<Stmt> block;
    while (!lexer->empty()) {
        std::optional<Stmt> out = iter_lexer(lexer);
        if (out) block.push_back(*out);
    }
    return block;
}
