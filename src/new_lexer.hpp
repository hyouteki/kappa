#ifndef KAPPA_LEXER_HPP_
#define KAPPA_LEXER_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <unordered_set>

class Loc;
class Token;
class Lexer;

std::string token_kind_to_str(const int);

class Loc {
    size_t row;
    size_t col;
public:
	Loc(const size_t row, const size_t col)
		: row(row), col(col) {}
    void print() const;
};

class Token {
public:
	enum Token_Kind {
    	tok_eof = -1,

		// keywords
		tok_fn = -2,
		tok_if = -3,
		tok_else = -4,
		tok_while = -5,
		tok_var = -6,
		tok_val = -7,

		// control flows
		tok_return = -8,
		tok_break = -9,
		tok_continue = -10,

		// datatypes
		tok_type_str = -11,
		tok_type_int = -12,
		tok_type_bool = -13,

		// vals
		tok_iden = -14,
		tok_str_lit = -15,
		tok_num = -16,
		tok_bool = -17
    };
	
	int kind;
    std::string str;
    int num;
	Loc loc;

	Token(const int kind, const std::string str, 
		const Loc loc): kind(kind), str(str), loc(loc) {}
	Token(const int kind, const int num, const Loc loc)
		: kind(kind), num(num), loc(loc) {}
    void print() const;
    bool equal(const Token) const;
    bool equal(const std::string) const;
    bool equal(const int) const;
};

class Lexer {
    std::vector<std::string> content;
    std::string filename;
    std::vector<Token> tokens;
public:
	Lexer(const std::vector<std::string> content, 
		const std::string filename, const std::vector<Token> tokens)
		: content(content), filename(filename), tokens(tokens) {}
    void print() const;
    bool empty() const;
    size_t size() const;
    Token at(size_t) const;
    void del_front();
    Token front() const;
    void gen_tokens();
    void assert_token_front() const;
    void assert_token_front(const Token) const;
    void assert_token_front(const std::string) const;
    void assert_token_front(const int) const;
    void assert_token_front(const std::vector<int>) const;
    bool is_token_front(const std::vector<int>) const;
    bool is_token_front(const std::string) const;
    bool is_token_front(const int) const;
    bool is_token_front(const std::unordered_set<int>) const;
};

#endif // KAPPA_LEXER_HPP_
