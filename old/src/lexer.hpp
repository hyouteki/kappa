#ifndef KAPPA_LEXER_HPP_
#define KAPPA_LEXER_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <unordered_set>

typedef enum {
    NAME,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_CURLY,
    CLOSE_CURLY,
    STR_LIT,
    COLON,
    COMMA,
    EQUAL,
    SEMI,
    PLUS,
    MINUS,
    MUL,
    DIV,
    MOD,
    POW,
    AND,
    OR,
    BIT_AND,
    BIT_OR,
    BIT_XOR,
    COMP_EQUAL, // ==
    COMP_NOT_EQUAL, // !=
    COMP_LT_EQUAL, // <=
    COMP_GT_EQUAL, // >=
    COMP_LT, // <
    COMP_GT, // >
    NOT,
} Lexeme_Kind;

typedef struct Location {
    size_t row;
    size_t col;
public:
    void print() const;
} Location;

typedef struct Lexeme {
    Lexeme_Kind kind;
    std::string str = "";
    Location loc = {};
public:
    void print() const;
    bool equal(const Lexeme) const;
    bool equal(const std::string) const;
    bool equal(const Lexeme_Kind) const;
    bool is_int() const;
} Lexeme;

typedef struct Lexer {
    std::vector<std::string> content;
    std::string filename;
    std::vector<Lexeme> lexemes = {};
public:
    void print() const;
    bool empty() const;
    size_t size() const;
    Lexeme at(size_t) const;
    void del_front();
    Lexeme front() const;
    void gen_lexemes();
    void assert_lexeme_front() const;
    void assert_lexeme_front(const Lexeme) const;
    void assert_lexeme_front(const std::string) const;
    void assert_lexeme_front(const Lexeme_Kind) const;
    void assert_lexeme_front(const std::vector<Lexeme_Kind>) const;
    bool is_lexeme_front(const std::vector<Lexeme_Kind>) const;
    bool is_lexeme_front(const std::string) const;
    bool is_lexeme_front(const Lexeme_Kind) const;
    bool is_lexeme_front(const std::unordered_set<Lexeme_Kind>) const;
} Lexer;

std::string lexeme_kind_to_str(const Lexeme_Kind);

#endif // KAPPA_LEXER_HPP_