#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "lexer.hpp"

std::vector<std::string> DEFAULT_TOKEN_STRS = {
    "(", ")", "{", ";", "}", ",", ":", "=", "+"};

std::unordered_map<Lexeme_Kind, std::string> lexeme_kind_str_map = {
    {NAME, "NAME"},
    {OPEN_PAREN, "OPEN_PAREN"},
    {CLOSE_PAREN, "CLOSE_PAREN"},
    {OPEN_CURLY, "OPEN_CURLY"},
    {CLOSE_CURLY, "CLOSE_CURLY"},
    {STR_LIT, "STR_LIT"},
    {SEMI, "SEMI"},
    {COMMA, "COMMA"},
    {COLON, "COLON"},
    {EQUAL, "EQUAL"},
    {PLUS, "PLUS"},
};

std::unordered_map<std::string, Lexeme_Kind> str_lexeme_kind_map = {
    {"(", OPEN_PAREN},
    {")", CLOSE_PAREN},
    {"{", OPEN_CURLY},
    {"}", CLOSE_CURLY},
    {";", SEMI},
    {",", COMMA},
    {":", COLON},
    {"=", EQUAL},
    {"+", PLUS},
};

bool Lexeme::is_int() const {
    if (this->kind != NAME) return false;
    for (char ch: this->str)
        if (!isdigit(ch)) return false;
    return true;
}

int strip_front(std::string *str) {
    std::string tmp = "";
    size_t col = 0;
    bool flag = false;
    for (char ch: *str) {
        if (flag) tmp.push_back(ch);
        else if (ch == ' ') ++col;
        else {
            tmp.push_back(ch);
            flag = true;
        }
    }
    *str = tmp;
    return col;
}

bool str_starts_with(const std::string str, const std::string target) {
    if (str.size() < target.size()) return false;
    for (size_t i = 0; i < target.size(); ++i)
        if (str[i] != target[i]) return false;
    return true;
}

std::string replace_str(
    const std::string initial,
    const std::string target,
    const std::string replacement
) {
    std::string result = initial;
    size_t pos = 0;
    while ((pos = result.find(target, pos)) != std::string::npos) {
        result.replace(pos, target.length(), replacement);
        pos += replacement.length();
    }
    return result;
}

std::string decode_str(const std::string& str) {
    std::string tmp = str;
    tmp = replace_str(tmp, "\\n", "\n");
    tmp = replace_str(tmp, "\\r", "\r");
    tmp = replace_str(tmp, "\\t", "\t");
    tmp = replace_str(tmp, "\\v", "\v");
    tmp = replace_str(tmp, "\\f", "\f");
    tmp = replace_str(tmp, "\\\\", "\\");
    tmp = replace_str(tmp, "\\'", "'");
    return tmp;
}

bool is_valid_name(const char ch) {
    return (
        ('0' <= ch && ch <= '9') ||
        ('a' <= ch && ch <= 'z') ||
        ('A' <= ch && ch <= 'Z') ||
        ch == '_'
    );
}

void Location::print() const {
    std::cout << this->row << ":" << this->col;
}

void Lexeme::print() const {
    std::cout << lexeme_kind_str_map.at(this->kind) << ":";
    this->loc.print(); std::cout << ": " << this->str;
}

bool Lexeme::equal(const Lexeme lexeme) const {
    return this->kind == lexeme.kind && this->str == lexeme.str;
}

bool Lexeme::equal(const std::string name) const {
    return this->str == name;
}

bool Lexeme::equal(const Lexeme_Kind kind) const {
    return this->kind == kind;
}

void Lexer::print() const {
    for (Lexeme lexeme: this->lexemes) {
        lexeme.print(); std::cout << std::endl;
    }
}

bool Lexer::empty() const {
    return this->lexemes.empty();
}

size_t Lexer::size() const {
    return this->lexemes.size();
}

Lexeme Lexer::at(size_t i) const {
    return this->lexemes[i];
}

void Lexer::del_front() {
    this->assert_lexeme_front();
    this->lexemes.erase(this->lexemes.begin());
}

Lexeme Lexer::front() const {
    this->assert_lexeme_front();
    return this->lexemes[0];
}

void Lexer::gen_lexemes() {
    std::vector<Lexeme> lexemes;
    if (this->content.empty()) return;
    for (size_t i = 0; i < this->content.size(); ++i) {
        size_t col = 0;
        while (this->content[i] != "") {
            col += strip_front(&this->content[i]);
            if (this->content[i] == "") continue;
            bool flag = false;
            for (std::string default_token: DEFAULT_TOKEN_STRS) {
                if (str_starts_with(this->content[i], default_token)) {
                    Lexeme lexeme = (Lexeme){
                        .kind = str_lexeme_kind_map.at(default_token),
                        .str = default_token,
                        .loc = (Location){.row = i+1, .col = col+1},
                    };
                    lexemes.push_back(lexeme);
                    this->content[i] = this->content[i].substr(default_token.size());
                    col += default_token.size();
                    flag = true;
                    break;
                }
            }
            if (flag) continue;
            size_t j = 0;
            for (; is_valid_name(this->content[i][j]); ++j);
            if (j != 0) {
                Lexeme lexeme = (Lexeme){
                    .kind = NAME,
                    .str = this->content[i].substr(0, j),
                    .loc = (Location){.row = i+1, .col = col+1},
                };
                lexemes.push_back(lexeme);
                this->content[i] = this->content[i].substr(j);
                col += j;
                continue;
            }
            if (this->content[i][0] == '"') {
                std::string tmp = "\"";
                for (size_t k = 1; k < this->content[i].size(); ++k) {
                    tmp.push_back(this->content[i][k]);
                    if (this->content[i][k] == '"') break;
                }
                if (tmp[tmp.size()-1] != '"') {
                    Location loc = (Location){.row = i+1, .col = col+tmp.size()+1};
                    std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
                    std::cerr << this->filename << ":";
                    loc.print();
                    std::cerr << ": ERROR: String literal not closed" << std::endl;
                    exit(1);
                }
                Lexeme lexeme = (Lexeme){
                    .kind = STR_LIT,
                    .str = decode_str(tmp),
                    .loc = (Location){.row = i+1, .col = col+1},
                };
                lexemes.push_back(lexeme);
                this->content[i] = this->content[i].substr(tmp.size());
                col += tmp.size();
                continue;
            }
            Location loc = (Location){.row = i+1, .col = col+1};
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
            std::cerr << this->filename << ":";
            loc.print();
            std::cerr << ": ERROR: Invalid token '";
            std::cerr << this->content[i][0] << "'" << std::endl;
            exit(1);
        }
    }
    this->lexemes = lexemes;
}

void Lexer::assert_lexeme_front() const {
    if (this->empty()) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << this->filename;
        std::cerr << ": ERROR: Expected token; got nothing" << std::endl;
        exit(1);
    }
}

void Lexer::assert_lexeme_front(const Lexeme lexeme) const {
    if (this->empty()) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << this->filename << ": ERROR: Expected token '";
        std::cerr << lexeme.str << "'; got nothing";
        exit(1);
    }
    if (!this->front().equal(lexeme)) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << this->filename << ":";
        this->front().loc.print();
        std::cerr << ": ERROR: Expected token '" << lexeme.str;
        std::cerr << "'; got '" << this->front().str << "'" << std::endl;
        exit(1);
    }
}

void Lexer::assert_lexeme_front(const std::string name) const {
    if (this->empty()) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << this->filename << ": ERROR: Expected token '";
        std::cerr << name << "'; got nothing";
        exit(1);
    }
    if (!this->front().equal(name)) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << this->filename << ":";
        this->front().loc.print();
        std::cerr << ": ERROR: Expected token '" << name;
        std::cerr << "'; got '" << this->front().str << "'" << std::endl;
        exit(1);
    }
}

void Lexer::assert_lexeme_front(const Lexeme_Kind kind) const {
    if (this->empty()) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << this->filename << ": ERROR: Expected token of kind '";
        std::cerr << lexeme_kind_str_map.at(kind) << "'; got nothing";
        exit(1);
    }
    if (!this->front().equal(kind)) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << this->filename << ":";
        this->front().loc.print();
        std::cerr << ": ERROR: Expected token of kind '";
        std::cerr << lexeme_kind_str_map.at(kind) << "'; got token '";
        std::cerr << this->front().str << "' of kind '";
        std::cerr << lexeme_kind_str_map.at(this->front().kind);
        std::cerr << "'" << std::endl;
        exit(1);
    }
}

void Lexer::assert_lexeme_front(const std::vector<Lexeme_Kind> kinds) const {
    if (this->size() < kinds.size()) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << this->filename << ": ERROR: Expected Token_Kind array size '";
        std::cerr << kinds.size() << "'; got '" << this->size() << "'";
        exit(1);
    }
    for (size_t i = 0; i < this->size(); ++i) {
        if (!this->at(i).equal(kinds[i])) {
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
            std::cerr << this->filename << ":";
            this->at(i).loc.print();
            std::cerr << ": ERROR: Expected token of kind '";
            std::cerr << lexeme_kind_str_map.at(kinds[i]) << "'; got token '";
            std::cerr << this->at(i).str << "' of kind '";
            std::cerr << lexeme_kind_str_map.at(this->at(i).kind);
            std::cerr << "'" << std::endl;
            exit(1);
        }
    }
}

bool Lexer::is_lexeme_front(const std::vector<Lexeme_Kind> kinds) const {
    if (this->size() < kinds.size()) return false;
    for (size_t i = 0; i < kinds.size(); ++i)
        if (!this->at(i).equal(kinds[i])) return false;
    return true;
}


bool Lexer::is_lexeme_front(const std::string str) const {
    return !this->empty() && this->front().equal(str);
}


bool Lexer::is_lexeme_front(const Lexeme_Kind kind) const {
    return !this->empty() && this->front().equal(kind);
}

bool Lexer::is_lexeme_front(const std::unordered_set<Lexeme_Kind> kinds) const {
    if (this->empty()) return false;
    for (Lexeme_Kind kind: kinds)
        if (this->is_lexeme_front(kind)) return true;
    return false;
}


std::string lexeme_kind_to_str(const Lexeme_Kind kind) {
    switch (kind) {
        case NAME: return "NAME";
        case OPEN_PAREN: return "OPEN_PAREN";
        case CLOSE_PAREN: return "CLOSE_PAREN";
        case OPEN_CURLY: return "OPEN_CURLY";
        case CLOSE_CURLY: return "CLOSE_CURLY";
        case STR_LIT: return "STR_LIT";
        case SEMI: return "SEMI";
        case COMMA: return "COMMA";
        case COLON: return "COLON";
        case EQUAL: return "EQUAL";
        case PLUS: return "PLUS";
    }
    return "";
}
