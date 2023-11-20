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
    "fun", "if", "else", "true", "false", "var", "int", "while", "return",
    "val", "bool", "str"
};

std::string Expr::str_val() const {
    if (this->kind != STR) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected Expr_Type `STR`; got '";
        std::cerr << this->kind << "'" << std::endl;
        exit(1);
    }
    return this->val.str_val;
}
bool Expr::bool_val() const {
    if (this->kind != BOOL) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected Expr_Type `BOOL`; got '";
        std::cerr << this->kind << "'" << std::endl;
        exit(1);
    }
    return this->val.bool_val;
}
Fun_Call Expr::fun_call() const {
    if (this->kind != FUN_CALL) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected Expr_Type `FUN_CALL`; got '";
        std::cerr << this->kind << "'" << std::endl;
        exit(1);
    }
    return this->val.fun_call;
}
int Expr::int_val() const {
    if (this->kind != INT) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected Expr_Type `INT`; got '";
        std::cerr << this->kind << "'" << std::endl;
        exit(1);
    }
    return this->val.int_val;
}

std::string Expr::str() const {
    switch (this->kind) {
        case ANY:
        case STR:
            return this->str_val();
        case BOOL:
            return (this->bool_val())? "true": "false";
        case INT:
            return std::to_string(this->int_val());
        default:
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":";
            std::cerr << __LINE__ << std::endl << "ERROR: Cannot stringify Expr_Kind '";
            std::cerr << this->kind << "'" << std::endl;
            exit(1);
    }
    return "";
}

void Expr::print() const {
    switch (this->kind) {
        case ANY:
        case STR:
            std::cout << this->str_val();
            break;
        case INT:
            std::cout << this->int_val();
            break;
        case BOOL:
            if (this->bool_val()) std::cout << "true";
            else std::cout << "false";
            break;
        case FUN_CALL:
            this->val.fun_call.print();
            break;
        default:
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
            std::cerr << "ERROR: Invalid Expr_Kind" << std::endl;
            exit(1);
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

Stmt::Stmt(const Var var) {
    this->kind = VAR;
    this->val.var = var;
}

Fun Stmt::fun() const {
    if (this->kind != FUN_DEF) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected Expr_Type `FUN_DEF`; got '";
        std::cerr << this->kind << "'" << std::endl;
        exit(1);
    }
    return this->val.fun;
}
If Stmt::if_cond() const {
    if (this->kind != IF) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected Expr_Type `IF`; got '";
        std::cerr << this->kind << "'" << std::endl;
        exit(1);
    }
    return this->val.if_cond;
}
Expr Stmt::expr() const {
    if (this->kind != EXPR) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected Expr_Type `EXPR`; got '";
        std::cerr << this->kind << "'" << std::endl;
        exit(1);
    }
    return this->val.expr;
}
Var Stmt::var() const {
    if (this->kind != VAR) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected Expr_Type `VAR`; got '";
        std::cerr << this->kind << "'" << std::endl;
        exit(1);
    }
    return this->val.var;
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
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
            std::cerr << "ERROR: Invalid Stmt_Kind" << std::endl;
            exit(1);
    }
}

std::optional<Expr> parse_expr(Lexer* lexer, bool error) {
    Expr* expr = new Expr();
    if (is_fun_call(lexer)) {
        Fun_Call fun_call = Fun_Call(lexer);
        expr->kind = FUN_CALL;
        expr->val.fun_call = fun_call;
        return *expr;
    }
    if (lexer->front().equal(STR_LIT)) {
        expr->kind = STR;
        expr->val.str_val = lexer->front().str.substr(1,
            lexer->front().str.size()-2);
        lexer->del_front();
        return *expr;
    }
    if (lexer->front().is_int()) {
        expr->kind = INT;
        expr->val.int_val = std::stoi(lexer->front().str);
        lexer->del_front();
        return *expr;
    }
    if (lexer->front().equal("true")) {
        expr->kind = BOOL;
        expr->val.bool_val = true;
        lexer->del_front();
        return *expr;
    }
    if (lexer->front().equal("false")) {
        expr->kind = BOOL;
        expr->val.bool_val = false;
        lexer->del_front();
        return *expr;
    }
    if (error) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << lexer->filename << ":"; lexer->front().loc.print();
        std::cerr << ": ERROR: Invalid expr '" << lexer->front().str << "'" << std::endl;
        exit(1);
    }
    return {};
}

Fun_Call::Fun_Call(Lexer* lexer) {
    this->name = lexer->front().str;
    lexer->del_front();
    lexer->del_front();
    while (!lexer->front().equal(CLOSE_PAREN)) {
        this->args.push_back(*parse_expr(lexer));
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

std::optional<Expr> Fun::eval(const Fun_Call fun_call) const {
    (void)fun_call;
    simul(this->block);
    return this->return_expr;
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
    else if (lexer->front().equal("var") || lexer->front().equal("val"))
        return *(new Stmt(assign_var(lexer)));
    std::optional<Expr> out = parse_expr(lexer, false);
    if (out) return *(new Stmt(*out));
    std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
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
            .kind = ANY,
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
    bool flag = true;
    while (!lexer->front().equal(CLOSE_CURLY)) {
        if (lexer->is_lexeme_front("return") && flag) {
            lexer->del_front();
            this->return_expr = parse_expr(lexer);
            flag = false;
        }
        std::optional<Stmt> out = iter_lexer(lexer);
        if (out && flag) this->block.push_back(*out);
        if (lexer->is_lexeme_front("return") && flag) {
            lexer->del_front();
            this->return_expr = parse_expr(lexer);
            flag = false;
        }
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

Var assign_var(Lexer* lexer) {
    Var var;
    if (lexer->is_lexeme_front("var")) var.mut = true;
    else if (lexer->is_lexeme_front("val")) var.mut = false;
    lexer->del_front();
    lexer->assert_lexeme_front(NAME);
    var.name = lexer->front().str;
    lexer->del_front();
    lexer->assert_lexeme_front(COLON);
    lexer->del_front();
    lexer->assert_lexeme_front(NAME);
    if (lexer->front().str == "str") var.type = STR;
    else if (lexer->front().str == "int") var.type = INT;
    else if (lexer->front().str == "bool") var.type = BOOL;
    else if (lexer->front().str == "bool") var.type = ANY;
    else {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << lexer->filename << ":"; lexer->front().loc.print();
        std::cerr << ": ERROR: Invalid datatype '" << lexer->front().str << "'" << std::endl;
        exit(1);
    }
    lexer->del_front();
    lexer->assert_lexeme_front(EQUAL);
    lexer->del_front();
    Expr expr = *parse_expr(lexer, true);
    if (var.type != expr.kind) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << lexer->filename << ":"; lexer->front().loc.print();
        std::cerr << ": ERROR: Type mismatch, expected type '" << expr_kind_to_str(var.type);
        std::cerr << "'; got '" << expr_kind_to_str(expr.kind) << "'" << std::endl;
        exit(1);
    }
    var.expr = expr;
    return var;
}

std::string expr_kind_to_str(const Expr_Kind kind) {
    switch (kind) {
        case STR: return "str";
        case INT: return "int";
        case BOOL: return "bool";
        case ANY: return "any";
        case FUN_CALL: return "fun_call";
        default:
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
            std::cerr << "ERROR: Invalid Expr_Kind " << kind << "'" << std::endl;
            exit(1);
    }
}
