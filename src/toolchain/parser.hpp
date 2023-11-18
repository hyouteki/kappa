#ifndef KAPPA_PARSER_HPP_
#define KAPPA_PARSER_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

typedef enum {
    STR_EXPR,
    FUN_CALL,
    ANY_EXPR,
} Expr_Kind;

typedef struct Expr Expr;

typedef struct Fun_Call {
    std::string name = "";
    std::vector<Expr> args = {};
public:
    Fun_Call();
    Fun_Call(Lexer *lexer);
    void print() const;
} Fun_Call;

typedef struct {
    std::string str_val = "";
    Fun_Call fun_call = {};
} Expr_Val;

struct Expr {
    Expr_Kind kind;
    Expr_Val val;
public:
    void print() const;
};

typedef struct Stmt {
    std::vector<Expr> stmt;
} Stmt;

typedef struct Fun {
    std::string name;
    std::vector<Expr> args;
    std::vector<Stmt> block;
public:
    Fun(Lexer *lexer);
    void print() const;
} Fun;

Expr iter_args(Lexer* lexer);
bool is_fun_call(const Lexer* lexer);
Fun_Call parse_fun_call(Lexer* lexer);
void parse_lexer(Lexer *lexer);

#endif // KAPPA_PARSER_HPP_
