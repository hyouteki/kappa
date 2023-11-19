#ifndef KAPPA_CPP_
#define KAPPA_CPP_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "lexer.hpp"
#include "parser.hpp"
#include "simulator.hpp"

std::vector<std::string> get_file_contents(std::string filename) {
    std::ifstream fd(filename);
    if (fd.fail()) {
        std::cerr << "BAD FILE :: Filename `" << filename;
        std::cerr << "` does not exist" << std::endl;
        exit(1);
    }
    std::string line;
    std::vector<std::string> lines;
    while (getline(fd, line)) lines.push_back(line);
    fd.close();
    return lines;
}

int main(int argc, char* argv[]) {
    std::string filename = (argc > 1)? std::string(argv[1]): "main.K";
    std::vector<std::string> content = get_file_contents(filename);
    Lexer lexer = (Lexer){.content = content, .filename = filename};
    lexer.gen_lexemes();
    // lexer.print();
    std::vector<Stmt> stmts = parse_lexer(&lexer);
    simul(stmts);
    return 0;
}

#endif // KAPPA_CPP_
