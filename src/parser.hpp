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
typedef struct Var Var;

typedef struct Fun_Call {
    std::string name = "";
    std::vector<Expr> args = {};
public:
    Fun_Call() {}
    Fun_Call(Lexer*);
    void print() const;
} Fun_Call;

typedef enum {
    STR,
    BOOL,
    INT,
    ANY,
    NULL_EXPR,
    FUN_CALL,
} Expr_Kind;

struct Expr {
    Expr_Kind kind;
    typedef struct {
        std::string str_val = "";
        bool bool_val = false;
        Fun_Call fun_call = {};
        int int_val = 0;
    } Expr_Val;
    Expr_Val val = {};
public:
    std::string str() const;
    void print() const;
    std::string str_val() const;
    bool bool_val() const;
    Fun_Call fun_call() const;
    int int_val() const;
};

typedef struct Fun {
    std::string name = "";
    Expr_Kind return_type = NULL_EXPR;
    std::vector<Var> args = {};
    std::vector<Stmt> block = {};
public:
    Fun() {}
    Fun(Lexer*);
    std::optional<Expr> eval(const Fun_Call fun_call) const;
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

struct Var {
    Expr_Kind type;
    bool mut = true;
    std::string name = "";
    std::optional<Expr> expr = {};
};

struct Stmt {
    typedef enum {
        FUN_DEF,
        IF,
        EXPR,
        VAR,
        RETURN,
    } Stmt_Kind;
    Stmt_Kind kind;
    typedef struct Stmt_Val {
        Fun fun;
        If if_cond;
        Expr expr;
        Var var;
    } Stmt_Val;
    Stmt_Val val;
public:
    Stmt(const Fun);
    Stmt(const If);
    Stmt(const Expr);
    Stmt(const Var);
    Fun fun() const;
    If if_cond() const;
    Expr expr() const;
    Var var() const;
    void print() const;
};

Var assign_var(Lexer*);
std::optional<Stmt> iter_lexer(Lexer*);
std::optional<Expr> parse_expr(Lexer*, bool = true);
bool is_fun_call(const Lexer*);
Fun_Call parse_fun_call(Lexer*);
std::vector<Stmt> parse_lexer(Lexer*);
std::string expr_kind_to_str(const Expr_Kind);
std::string stmt_kind_to_str(const Stmt::Stmt_Kind);
Expr_Kind str_to_expr_kind(Lexer*);

#endif // KAPPA_PARSER_HPP_
