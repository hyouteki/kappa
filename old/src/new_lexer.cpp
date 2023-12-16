#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "new_lexer.hpp"

int strip_front(std::string*);
bool starts_with(const std::string, const std::string);
std::string replace_str(const std::string, const std::string, 
	const std::string);

std::unordered_map<std::string, int> str_to_token_kind = {
	{"fn", Token::tok_fn}, {"if", Token::tok_if}, {"else", Token::tok_else}, 
	{"while", Token::tok_while}, {"var", Token::tok_var}, {"val", Token::tok_val}, 
	{"return", Token::tok_return}, {"break", Token::tok_break}, 
	{"continue", Token::tok_continue}, {"str", Token::tok_type_str}, 
	{"int", Token::tok_type_int}, {"bool", Token::tok_type_bool},
	{"true", Token::tok_bool}, {"false", Token::tok_bool}};

bool starts_with(const std::string str, const std::string target) {
    if (str.size() < target.size()) return false;
    for (size_t i = 0; i < target.size(); ++i)
        if (str[i] != target[i]) return false;
    return true;
}

std::string replace_str(const std::string initial,
    const std::string target, const std::string replacement) {
    std::string result = initial;
    size_t pos = 0;
    while ((pos = result.find(target, pos)) != std::string::npos) {
        result.replace(pos, target.length(), replacement);
        pos += replacement.length();
    }
    return result;
}

void Loc::print() const {
    std::cout << this->row << ":" << this->col;
}

void Token::print() const {
    this->loc.print(); std::cout << "\t";
	std::cout << this->str << ": ";
	std::cout << token_kind_to_str(this->kind);
}

bool Token::equal(const Token token) const {
    return this->kind == token.kind && this->str == token.str;
}

bool Token::equal(const std::string name) const {
    return this->str == name;
}

bool Token::equal(const int kind) const {
    return this->kind == kind;
}

void Lexer::print() const {
    for (Token token: this->tokens) {
		token.print(); 
		std::cout << std::endl;
	}
}

bool Lexer::empty() const {
    return this->tokens.empty();
}

size_t Lexer::size() const {
    return this->tokens.size();
}

Token Lexer::at(size_t i) const {
    return this->tokens[i];
}

void Lexer::del_front() {
    this->assert_token_front();
    this->tokens.erase(this->tokens.begin());
}

Token Lexer::front() const {
    this->assert_token_front();
    return this->tokens[0];
}

void Lexer::gen_tokens() {
	std::vector<Token> tokens;
    if (this->content.empty()) return;
    for (size_t i = 0; i < this->content.size(); ++i) {
        size_t col = 0;
        while (this->content[i] != "") {
			{
				int x = 0;
				for (char ch: this->content[i]) {
					if (ch != ' ' && ch != '\t') break;
					++x;
				}
				col += x;
				this->content[i] = this->content[i].substr(x);
			}
			if (this->content[i] == "") continue;
        
			int j = 0;
			char ch = this->content[i][j];	
				
			if (isalnum(ch) || ch == '_') {
				std::string iden;
				while (isalnum(ch) || ch == '_') {
					iden.push_back(ch);
					ch = this->content[i][++j];
				}
				Token token = (str_to_token_kind.find(iden) != str_to_token_kind.end())?
					Token(str_to_token_kind[iden], iden, Loc(i+1, col+1))
					: Token(Token::tok_iden, iden, Loc(i+1, col+1));
				tokens.push_back(token);
				col += j;
				this->content[i] = this->content[i].substr(j);
				continue;
			} 

			if (isdigit(ch)) {
				int num = 0;
				while (isdigit(ch)) {
					num = num*10 + ch - '0';
					ch = this->content[i][++j];
				}
				Token token = Token(Token::tok_num, num, Loc(i+1, col+1));
				tokens.push_back(token);
				col += j;
				this->content[i] = this->content[i].substr(j);
				continue;
			}

			if (ch == '"') {
				std::string iden = "\"";
				for (size_t k = 1; k < this->content[i].size(); ++k) {
					iden.push_back(this->content[i][k]);
					if (this->content[i][k] == '"') break;
				}	
				if (iden[iden.size()-1] != '"') {	
					std::cerr << __FILE__ << ":" << __FUNCTION__ << ":";
					std::cerr << __LINE__ << std::endl << this->filename;
					std::cerr << ":"; Loc(i+1, col+iden.size()+1).print();
					std::cerr << ": error: string literal not closed" << std::endl;
					exit(1);
				}
				Token token = Token(Token::tok_str_lit, iden, Loc(i+1, col+1));
				tokens.push_back(token);
				col += iden.size();
				this->content[i] = this->content[i].substr(iden.size());
				continue;
			}
				
			{
				std::string iden = "";
				iden.push_back(ch);
				Token token = Token(ch, iden, Loc(i+1, col+1));
				++col;
				tokens.push_back(token);
				this->content[i] = this->content[i].substr(1);
				continue;
			}	
		}
	}
	Token token = Token(Token::tok_eof, "", Loc(0, 0));
	tokens.push_back(token);
    this->tokens = tokens;
}

void Lexer::assert_token_front() const {
    if (this->empty()) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << this->filename << ": error: expected a token" << std::endl;
        exit(1);
    }
}

void Lexer::assert_token_front(const Token token) const {
    if (this->empty()) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << this->filename << ": error: expected token '";
		std::cerr << token.str << "'" << std::endl;
        exit(1);
    }
    if (!this->front().equal(token)) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << this->filename << ":"; this->front().loc.print();
        std::cerr << ": error: expected token '" << token.str;
        std::cerr << "'; got '" << this->front().str << "'" << std::endl;
        exit(1);
    }
}

void Lexer::assert_token_front(const std::string name) const {
    if (this->empty()) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << this->filename << ": error: expected token '" << name << "'";
        exit(1);
    }
    if (!this->front().equal(name)) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << this->filename << ":"; this->front().loc.print();
        std::cerr << ": error: expected token '" << name;
        std::cerr << "'; got '" << this->front().str << "'" << std::endl;
        exit(1);
    }
}

void Lexer::assert_token_front(const int kind) const {
    if (this->empty()) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << this->filename << ": error: expected token of kind '";
        std::cerr << token_kind_to_str(kind) << std::endl;
        exit(1);
    }
    if (!this->front().equal(kind)) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << this->filename << ":"; this->front().loc.print();
        std::cerr << ": error: expected token of kind '";
        std::cerr << token_kind_to_str(kind) << "'; got token '";
        std::cerr << this->front().str << "' of kind '";
        std::cerr << token_kind_to_str(this->front().kind);
        std::cerr << "'" << std::endl;
        exit(1);
    }
}

void Lexer::assert_token_front(const std::vector<int> kinds) const {
    if (this->size() < kinds.size()) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << this->filename << ": error: expected int array size '";
        std::cerr << kinds.size() << "'; got '" << this->size() << "'";
        exit(1);
    }
    for (size_t i = 0; i < this->size(); ++i) {
        if (!this->at(i).equal(kinds[i])) {
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
            std::cerr << this->filename << ":"; this->at(i).loc.print();
            std::cerr << ": error: expected token of kind '";
            std::cerr << token_kind_to_str(kinds[i]) << "'; got token '";
            std::cerr << this->at(i).str << "' of kind '";
            std::cerr << token_kind_to_str(this->at(i).kind) << "'" << std::endl;
            exit(1);
        }
    }
}

bool Lexer::is_token_front(const std::vector<int> kinds) const {
    if (this->size() < kinds.size()) return false;
    for (size_t i = 0; i < kinds.size(); ++i)
        if (!this->at(i).equal(kinds[i])) return false;
    return true;
}

bool Lexer::is_token_front(const std::string str) const {
    return !this->empty() && this->front().equal(str);
}

bool Lexer::is_token_front(const int kind) const {
    return !this->empty() && this->front().equal(kind);
}

bool Lexer::is_token_front(const std::unordered_set<int> kinds) const {
    if (this->empty()) return false;
    for (int kind: kinds)
        if (this->is_token_front(kind)) return true;
    return false;
}

std::string token_kind_to_str(const int kind) {
    switch (kind) {
        case Token::tok_eof: return "tok_eof";
        
		// keywords
		case Token::tok_fn: return "tok_fn";
        case Token::tok_if: return "tok_if";
        case Token::tok_else: return "tok_else";
        case Token::tok_while: return "tok_while";
        case Token::tok_var: return "tok_var";
        case Token::tok_val: return "tok_val";
        
		// control flows
		case Token::tok_return: return "tok_return";
        case Token::tok_break: return "tok_break";
        case Token::tok_continue: return "tok_continue";
        
		// datatypes
		case Token::tok_type_str:  return "tok_type_str";
        case Token::tok_type_int: return "tok_type_int";
        case Token::tok_type_bool: return "tok_type_bool";
        
		// vals
		case Token::tok_iden: return "tok_iden";
        case Token::tok_str_lit: return "tok_str_lit";
        case Token::tok_num: return "tok_num";
        case Token::tok_bool: return "tok_bool";
    }

	std::string str = "tok_";
    str.push_back(kind);
	return str;
}
