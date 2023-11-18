#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "lexer.hpp"
#include "parser.hpp"

Expr parse_fun_call(Lexer *lexer) {
    lexer->assert_lexeme_front("fun");
    lexer->del_front();
    lexer->assert_lexeme_front(NAME);
    std::string fun_name = lexer->lexemes[0].str;
    lexer->del_front();
    lexer->assert_lexeme_front(OPEN_PAREN);
    return {};
}
