#ifndef KAPPA_LEXER_HPP_
#define KAPPA_LEXER_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

typedef enum {
    NAME,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_CURLY,
    CLOSE_CURLY,
    STR_LIT,
    COMMA,
    SEMI
} Lexeme_Kind;

typedef struct Location {
    size_t row;
    size_t col;
public:
    void print() const;
} Location;

typedef struct Lexeme {
    Lexeme_Kind kind;
    std::string str;
    Location loc = {};
public:
    void print() const;
    bool equal(const Lexeme) const;
    bool equal(const std::string) const;
    bool equal(const Lexeme_Kind) const;
} Lexeme;

typedef struct Lexer {
    std::vector<std::string> content;
    std::string filename;
    std::vector<Lexeme> lexemes = {};
public:
    void gen_lexemes();
    void assert_lexeme_front() const;
    void assert_lexeme_front(const Lexeme) const;
    void assert_lexeme_front(const std::string) const;
    void assert_lexeme_front(const Lexeme_Kind) const;
    void del_front();
    Lexeme front() const;
} Lexer;

#endif // KAPPA_LEXER_HPP_
