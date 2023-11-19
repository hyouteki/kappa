#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>
#include "lexer.hpp"
#include "parser.hpp"
#include "simulator.hpp"

std::unordered_set<std::string> reserved_strs = {
    "fun", "if", "else", "true", "false", "var", "int", "while"
};

std::string Expr::str() const {
    switch (this->kind) {
        case Expr::ANY:
        case Expr::STR:
            return this->val.str_val;
        case Expr::BOOL:
            return (this->val.bool_val)? "true": "false";
        default:
            std::cerr << "Cannot `stringify` Expr_Kind '";
            std::cerr << this->kind << "'" << std::endl;
    }
    return "";
}

void Expr::print() const {
    switch (this->kind) {
        case Expr::ANY:
        case Expr::STR:
            std::cout << this->val.str_val;
            break;
        case Expr::BOOL:
            if (this->val.bool_val) std::cout << "true";
            else std::cout << "false";
            break;
        case Expr::FUN_CALL:
            this->val.fun_call.print();
            break;
        default:
            std::cerr << "Invalid Expr_Kind" <<std::endl;
    }
}

If::If(Lexer* lexer) {
    lexer->assert_lexeme_front("if");
    lexer->del_front();
    lexer->assert_lexeme_front(OPEN_PAREN);
    lexer->del_front();
    std::optional<Stmt> tmp = iter_lexer(lexer);
    this->condition = (tmp)? new Stmt(*tmp): nullptr;
    lexer->assert_lexeme_front(CLOSE_PAREN);
    lexer->del_front();
    lexer->assert_lexeme_front(OPEN_CURLY);
    lexer->del_front();
    while (!lexer->front().equal(CLOSE_CURLY)) {
        std::optional<Stmt> out = iter_lexer(lexer);
        if (out) this->then_block.push_back(*out);
    }
    lexer->del_front();
    if (lexer->is_lexeme_front("else")) {
        lexer->del_front();
        lexer->assert_lexeme_front(OPEN_CURLY);
        lexer->del_front();
        while (!lexer->front().equal(CLOSE_CURLY)) {
            std::optional<Stmt> out = iter_lexer(lexer);
            if (out) this->else_block.push_back(*out);
        }
        lexer->del_front();
    }
}

void If::print() const {
    std::cout << "if (";
    this->condition->print();
    std::cout << ") {";
    for (size_t i = 0; i < this->then_block.size(); ++i) {
        this->then_block[i].print();
        if (i < this->then_block.size()-1) std::cout << std::endl;
    }
    std::cout << "}";
    if (this->else_block.empty()) return;
    std::cout << " else {";
    for (size_t i = 0; i < this->else_block.size(); ++i) {
        this->else_block[i].print();
        if (i < this->else_block.size()-1) std::cout << std::endl;
    }
    std::cout << "}";
}

Stmt::Stmt(const Fun fun) {
    this->kind = FUN_DEF;
    this->val.fun = fun;
}

Stmt::Stmt(const If if_cond) {
    this->kind = IF;
    this->val.if_cond = if_cond;
}

Stmt::Stmt(const Expr expr) {
    this->kind = EXPR;
    this->val.expr = expr;
}

void Stmt::print() const {
    switch (this->kind) {
        case FUN_DEF:
            this->val.fun.print();
            break;
        case EXPR:
            this->val.expr.print();
            break;
        case IF:
            this->val.if_cond.print();
            break;
        default:
            std::cerr << "Invalid Stmt_Kind" << std::endl;
            exit(1);
    }
}

std::optional<Expr> iter_args(Lexer* lexer, bool error) {
    Expr* expr = new Expr();
    if (is_fun_call(lexer)) {
        Fun_Call fun_call = Fun_Call(lexer);
        expr->kind = Expr::FUN_CALL;
        expr->val.fun_call = fun_call;
        return *expr;
    }
    if (lexer->front().equal(STR_LIT)) {
        expr->kind = Expr::STR;
        expr->val.str_val = lexer->front().str.substr(1,
            lexer->front().str.size()-2);
        lexer->del_front();
        return *expr;
    }
    if (lexer->front().equal("true")) {
        expr->kind = Expr::BOOL;
        expr->val.bool_val = true;
        lexer->del_front();
        return *expr;
    }
    if (lexer->front().equal("false")) {
        expr->kind = Expr::BOOL;
        expr->val.bool_val = false;
        lexer->del_front();
        return *expr;
    }
    if (error) {
        std::cerr << lexer->filename << ":"; lexer->front().loc.print();
        std::cerr << ": ERROR: Invalid argument '" << lexer->front().str << "'" << std::endl;
        exit(1);
    }
    return {};
}

Fun_Call::Fun_Call(Lexer* lexer) {
    this->name = lexer->front().str;
    lexer->del_front();
    lexer->del_front();
    while (!lexer->front().equal(CLOSE_PAREN)) {
        this->args.push_back(*iter_args(lexer));
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

std::string Fun::sign() const {
    std::string sign = "";
    sign += this->name;
    sign.push_back('(');
    for (size_t i = 0; i < this->args.size(); ++i) {
        sign += this->args[i].str();
        if (i < this->args.size()-1) sign.push_back(',');
    }
    sign.push_back(')');
    return sign;
}

void Fun::print() const {
    std::cout << "fun " << this->name << "(";
    for (size_t i = 0; i < this->args.size(); ++i) {
        this->args[i].print();
        if (i < this->args.size()-1) std::cout << ", ";
    }
    std::cout << ") {}";
}

void Fun::execute(const Fun_Call fun_call) const {
    (void)fun_call;
    simul(this->block);
}

bool is_fun_call(const Lexer* lexer) {
    std::vector<Lexeme_Kind> regex = {NAME, OPEN_PAREN};
    return (lexer->is_lexeme_front(regex) &&
        reserved_strs.find(lexer->front().str) == reserved_strs.end());
}

std::optional<Stmt> iter_lexer(Lexer* lexer) {
    if (lexer->front().equal(SEMI)) {
        lexer->del_front();
        return {};
    }
    else if (lexer->front().equal("fun")) return *(new Stmt(Fun(lexer)));
    else if (lexer->front().equal("if")) return *(new Stmt(If(lexer)));
    std::optional<Expr> out = iter_args(lexer, false);
    if (out) return *(new Stmt(*out));
    std::cerr << lexer->filename << ":"; lexer->front().loc.print();
    std::cerr << ": ERROR: Invalid token '" << lexer->front().str << "'" << std::endl;
    exit(1);
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
