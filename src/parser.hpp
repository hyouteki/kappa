#ifndef KAPPA_PARSER_HPP_
#define KAPPA_PARSER_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <optional>
#include "lexer.hpp"

typedef struct Expr Expr;
typedef struct Stmt Stmt;
typedef struct If If;

typedef struct Fun_Call {
    std::string name = "";
    std::vector<Expr> args = {};
public:
    Fun_Call() {}
    Fun_Call(Lexer*);
    void print() const;
} Fun_Call;

struct Expr {
    typedef enum {
        STR,
        BOOL,
        ANY,
        FUN_CALL,
    } Expr_Kind;
    Expr_Kind kind;
    typedef struct {
        std::string str_val = "";
        bool bool_val = false;
        Fun_Call fun_call = {};
    } Expr_Val;
    Expr_Val val = {};
public:
    std::string str() const;
    void print() const;
};

typedef struct Fun {
    std::string name = "";
    std::vector<Expr> args = {};
    std::vector<Stmt> block = {};
public:
    Fun() {}
    Fun(Lexer*);
    void execute(const Fun_Call fun_call) const;
    std::string sign() const;
    void print() const;
} Fun;

struct If {
    Stmt* condition;
    std::vector<Stmt> then_block = {};
    std::vector<Stmt> else_block = {};
public:
    If(): condition(nullptr), then_block(), else_block() {}
    If(Lexer*);
    void print() const;
};

struct Stmt {
    typedef enum {
        FUN_DEF,
        IF,
        EXPR,
    } Stmt_Kind;
    Stmt_Kind kind;
    typedef struct Stmt_Val {
        Fun fun;
        If if_cond;
        Expr expr;
    } Stmt_Val;
    Stmt_Val val;
public:
    Stmt(const Fun);
    Stmt(const If);
    Stmt(const Expr);
    void print() const;
};

std::optional<Stmt> iter_lexer(Lexer*);
std::optional<Expr> iter_args(Lexer*, bool = true);
bool is_fun_call(const Lexer*);
Fun_Call parse_fun_call(Lexer*);
std::vector<Stmt> parse_lexer(Lexer*);

#endif // KAPPA_PARSER_HPP_
