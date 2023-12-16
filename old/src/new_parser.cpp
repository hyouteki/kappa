#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <utility>
#include <iostream>
#include "new_parser.hpp"

Expr get_default_expr(const std::string);
std::pair<std::string, Expr> get_name_and_expr(Lexer*);

Expr get_default_expr(const std::string str) {
	if (str == "str") return Str_Lit_Expr("");
	if (str == "int") return Num_Expr(0);
	if (str == "bool") return Bool_Expr("false");
	std::cerr << __FILE__ << ":" << __FUNCTION__ << ":";
	std::cerr << __LINE__ << std::endl;
	std::cerr << "error: invalid data type" << std::endl;
	exit(1);
}

std::pair<std::string, Expr> get_name_and_expr(Lexer* lexer) {
	lexer->assert_token_front(Token::tok_iden);
	std::string arg = lexer->front().str;
	lexer->del_front();
	lexer->assert_token_front(':');
	lexer->del_front();
	lexer->assert_token_front();
	Expr _expr = get_default_expr(lexer->front().str);
	lexer->del_front();
	return std::make_pair(arg, _expr);
}

std::optional<Expr> parse_expr(Lexer* lexer) {
	// TODO: complete this
	(void) lexer;
	return std::nullopt;
}

std::optional<Num_Expr> parse_num_expr(Lexer* lexer) {
	if (lexer->front().kind != Token::tok_num) return std::nullopt;
	Num_Expr ret = Num_Expr(lexer->front().num);
	lexer->del_front();
	return ret;
}

std::optional<Var_Expr> parse_var_expr(Lexer* lexer) {
	if (lexer->front().kind != Token::tok_iden) return std::nullopt;
	Var_Expr ret = Var_Expr(lexer->front().str);
	lexer->del_front();
	return ret;
}

std::optional<Bool_Expr> parse_bool_expr(Lexer* lexer) {
	if (lexer->front().kind != Token::tok_bool) return std::nullopt;
	Bool_Expr ret = Bool_Expr(lexer->front().str == "true");
	lexer->del_front();
	return ret;
}

std::optional<Str_Lit_Expr> parse_str_lit_expr(Lexer* lexer) {
	if (lexer->front().kind != Token::tok_str_lit) return std::nullopt;
	Str_Lit_Expr ret = Str_Lit_Expr(lexer->front().str);
	lexer->del_front();
	return ret;
}

std::optional<Bin_Expr> parse_bin_expr(Lexer* lexer) {
	// TODO: complete this
	(void) lexer;
	return std::nullopt;
}

std::optional<Call_Expr> parse_call_expr(Lexer* lexer) {
	// TODO: complete this
	(void) lexer;
	return std::nullopt;
}

Block parse_block(Lexer* lexer) {
	lexer->assert_token_front('{');
	lexer->del_front();
	std::vector<Stmt> stmts;
	std::optional<Stmt> stmt = parse_stmt(lexer);
	while (stmt) {
		stmts.push_back(*stmt);
		stmt = parse_stmt(lexer);
	}
	lexer->assert_token_front('}');
	lexer->del_front();
	return Block(stmts, std::unordered_map<std::string, Expr>());
}

std::optional<Stmt> parse_stmt(Lexer* lexer) {
	// TODO: complete this
	(void) lexer;
	return std::nullopt;
}

std::optional<Fun_Def_Stmt> parse_fun_def_stmt(Lexer* lexer) {
	lexer->assert_token_front(Token::tok_fn);
	lexer->del_front();
	lexer->assert_token_front(Token::tok_iden);
	std::string fun_name = lexer->front().str;
	lexer->del_front();
	lexer->assert_token_front('(');
	lexer->del_front();
	std::vector<std::string> args;
	std::unordered_map<std::string, Expr> vars;
	while (lexer->front().kind != ')') {
	lexer->assert_token_front(Token::tok_iden);
		auto [arg, _expr] = get_name_and_expr(lexer);
		args.push_back(arg);
		vars[arg] = _expr;
		if (lexer->is_token_front(')')) break;
		lexer->assert_token_front(',');
		lexer->del_front();
	}
	lexer->del_front();
	lexer->assert_token_front(':');
	lexer->del_front();
	lexer->assert_token_front();
	Expr return_type = get_default_expr(lexer->front().str);
	lexer->del_front();
	Block block = parse_block(lexer);
	block.capture_vars(vars);
	return Fun_Def_Stmt(fun_name, args, return_type, block);
}

std::optional<If_Stmt> parse_if_stmt(Lexer* lexer) {
	// TODO: complete this
	(void) lexer;
	return std::nullopt;
}

std::optional<While_Stmt> parse_while_stmt(Lexer* lexer) {
	// TODO: complete this
	(void) lexer;
	return std::nullopt;
}

void Block::capture_vars(std::unordered_map<std::string, Expr> vars) {
	for (auto [_var, _expr]: vars) this->vars[_var] = _expr;
}

int main() {
	return 0;
}
