#ifndef KAPPA_PARSER_HPP_
#define KAPPA_PARSER_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <optional>

typedef struct Expr Expr;

typedef struct Fun_Call {
    std::string name = "";
    std::vector<Expr> args = {};
public:
    Fun_Call();
    Fun_Call(Lexer *lexer);
    void print() const;
} Fun_Call;

struct Expr {
    typedef enum {
        STR,
        FUN_CALL,
        ANY,
    } Expr_Kind;
    Expr_Kind kind;
    typedef struct {
        std::string str_val = "";
        Fun_Call fun_call = {};
    } Expr_Val;
    Expr_Val val = {};
public:
    void print() const;
};

typedef struct Stmt Stmt;

typedef struct Fun {
    std::string name = "";
    std::vector<Expr> args = {};
    std::vector<Stmt> block = {};
public:
    Fun() {}
    Fun(Lexer *lexer);
    void print() const;
} Fun;

struct Stmt {
    typedef enum {
        FUN_DEF,
        EXPR,
    } Stmt_Kind;
    Stmt_Kind kind;
    typedef struct Stmt_Val {
        Fun fun;
        Expr expr;
    } Stmt_Val;
    Stmt_Val val;
    Stmt(const Fun);
    Stmt(const Expr);
};

std::optional<Stmt> iter_lexer(Lexer*);
Expr iter_args(Lexer*);
bool is_fun_call(const Lexer*);
Fun_Call parse_fun_call(Lexer*);
std::vector<Stmt> parse_lexer(Lexer*);

#endif // KAPPA_PARSER_HPP_
