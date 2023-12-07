#ifndef KAPPA_PARSER_HPP_
#define KAPPA_PARSER_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include "lexer.hpp"

class Expr;
class Num_Expr;
class Var_Expr;
class Bin_Expr;
class Call_Expr;
class Block;
class Stmt;
class Fun_Def_Stmt;
class If_Stmt;
class While_Stmt;
class Return_Stmt;
  
std::optional<Expr> parse_expr(Lexer*);
std::optional<Num_Expr> parse_num_expr(Lexer*);
std::optional<Var_Expr> parse_var_expr(Lexer*);
std::optional<Bin_Expr> parse_bin_expr(Lexer*);
std::optional<Call_Expr> parse_call_expr(Lexer*);
std::optional<Block> parse_block(Lexer*);
std::optional<Stmt> parse_stmt(Lexer*);
std::optional<Fun_Def_Stmt> parse_fun_def_stmt(Lexer*);
std::optional<If_Stmt> parse_if_stmt(Lexer*);
std::optional<While_Stmt> parse_while_stmt(Lexer*);
std::optional<Return_Stmt> parse_return_stmt(Lexer*);

class Expr {
public:
	virtual ~Expr() = default;
	virtual void print() const {}
};

class Num_Expr: public Expr {
	int val;
public:
	Num_Expr(const int val): val(val) {}
	void print() const override;
};

class Var_Expr: public Expr {
	std::string name;
public:
	Var_Expr(const std::string &name): name(name) {}
	void print() const override;
};

class Bin_Expr: public Expr {
	Lexeme_Kind op;
	Expr left, right;
public:
	Bin_Expr(const Lexeme_Kind op, const Expr left, 
		const Expr right): op(op), left(left), right(right) {}
	void print() const override;
};

class Call_Expr: public Expr {
	std::string name;
	std::vector<Expr> args;
public:
	Call_Expr(const std::string name, 
		const std::vector<Expr> args): name(name), args(args) {}
	void print() const override;
};

class Block {
	std::vector<Stmt> stmts;
	std::unordered_map<std::string, Expr> vars;
public:
	Block(const std::vector<Stmt> stmts, 
		const std::unordered_map<std::string, 
		Expr> vars): stmts(stmts), vars(vars) {};
};

class Stmt {
public:
	virtual ~Stmt() = default;
	virtual void print() const {}
};

class Fun_Def_Stmt: public Stmt {
	std::string name;
	std::vector<Expr> args;
	Expr return_expr;
	Block block;
public:
	Fun_Def_Stmt(const std::string name, 
		const std::vector<Expr> args, 
		const Expr return_expr, const Block block)
		: name(name), args(args), 
		return_expr(return_expr), block(block) {}
	void print() const override;
};

class If_Stmt: public Stmt {
	Expr condition;
	Block then_block;
	Block else_block;
public:
	If_Stmt(const Expr condition, const Block then_block, 
		const Block else_block): condition(condition), 
		then_block(then_block), else_block(else_block) {}
	void print() const override;
};

class While_Stmt: public Stmt {
	Expr condition;
	Block block;
public:
	While_Stmt(const Expr condition, const Block block): 
		condition(condition), block(block) {}
	void print() const override;
};

class Return_Stmt: public Stmt {
	Expr expr;
public:
	Return_Stmt(const Expr expr): expr(expr) {}
	void print() const override;
};

#endif // KAPPA_PARSER_HPP_
