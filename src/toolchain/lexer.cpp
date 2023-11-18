#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "lexer.hpp"

std::vector<std::string> DEFAULT_TOKEN_STRS = {
    "fun", "(", ")", "{", ";", "}"
};

std::unordered_map<Lexeme_Kind, std::string> lexeme_kind_str_map = {
    {NAME, "NAME"},
    {OPEN_PAREN, "OPEN_PAREN"},
    {CLOSE_PAREN, "CLOSE_PAREN"},
    {OPEN_CURLY, "OPEN_CURLY"},
    {CLOSE_CURLY, "CLOSE_CURLY"},
    {STR_LIT, "STR_LIT"},
    {SEMI, "SEMI"},
};

std::unordered_map<std::string, Lexeme_Kind> str_lexeme_kind_map = {
    {"fun", NAME},
    {"(", OPEN_PAREN},
    {")", CLOSE_PAREN},
    {"{", OPEN_CURLY},
    {"}", CLOSE_CURLY},
    {";", SEMI},
};

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
    this->loc.print();
    std::cout << ": " << this->str;
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
                    std::cerr << this->filename << ":";
                    loc.print();
                    std::cerr << ": ERROR: String literal not closed" << std::endl;
                    exit(1);
                }
                Lexeme lexeme = (Lexeme){
                    .kind = STR_LIT,
                    .str = tmp,
                    .loc = (Location){.row = i+1, .col = col+1},
                };
                lexemes.push_back(lexeme);
                this->content[i] = this->content[i].substr(tmp.size());
                col += tmp.size();
                continue;
            }
            Location loc = (Location){.row = i+1, .col = col+1};
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
    if (this->lexemes.empty()) {
        std::cerr << this->filename;
        std::cerr << ": ERROR: Expected token; Got nothing";
        exit(1);
    }
}

void Lexer::assert_lexeme_front(const Lexeme lexeme) const {
    if (this->lexemes.empty()) {
        std::cerr << this->filename << ": ERROR: Expected token '";
        std::cerr << lexeme.str << "'; Got nothing";
        exit(1);
    }
    if (!this->lexemes[0].equal(lexeme)) {
        std::cerr << this->filename << ":";
        this->lexemes[0].loc.print();
        std::cerr << ": ERROR: Expected token '" << lexeme.str;
        std::cerr << "'; Got '" << this->lexemes[0].str << "'" << std::endl;
        exit(1);
    }
}

void Lexer::assert_lexeme_front(const std::string name) const {
    if (this->lexemes.empty()) {
        std::cerr << this->filename << ": ERROR: Expected token '";
        std::cerr << name << "'; Got nothing";
        exit(1);
    }
    if (!this->lexemes[0].equal(name)) {
        std::cerr << this->filename << ":";
        this->lexemes[0].loc.print();
        std::cerr << ": ERROR: Expected token '" << name;
        std::cerr << "'; Got '" << this->lexemes[0].str << "'" << std::endl;
        exit(1);
    }
}

void Lexer::assert_lexeme_front(const Lexeme_Kind kind) const {
    if (this->lexemes.empty()) {
        std::cerr << this->filename << ": ERROR: Expected token of kind '";
        std::cerr << lexeme_kind_str_map.at(kind) << "'; Got nothing";
        exit(1);
    }
    if (!this->lexemes[0].equal(kind)) {
        std::cerr << this->filename << ":";
        this->lexemes[0].loc.print();
        std::cerr << ": ERROR: Expected token of kind '";
        std::cerr << lexeme_kind_str_map.at(kind) << "'; Got token '";
        std::cerr << this->lexemes[0].str << "' of kind '";
        std::cerr << lexeme_kind_str_map.at(this->lexemes[0].kind);
        std::cerr << "'" << std::endl;
        exit(1);
    }
}

void Lexer::del_front() {
    this->assert_lexeme_front();
    this->lexemes.erase(this->lexemes.begin());
}
