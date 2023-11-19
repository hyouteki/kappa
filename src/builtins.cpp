#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "parser.hpp"
#include "builtins.hpp"

std::unordered_set<std::string> builtins = {
    "print",
};

void print(Fun_Call fun_call) {
    for (size_t i = 0; i < fun_call.args.size(); ++i) {
        fun_call.args[i].print();
        if (i < fun_call.args.size()-1) std::cout << " ";
    }
}

bool mapper(Fun_Call fun_call) {
    if (builtins.find(fun_call.name) == builtins.end()) return false;
    if (fun_call.name == "print") print(fun_call);
    else {
        std::cerr << "Invalid buitlin method" << std::endl;
        exit(1);
    }
    return true;
}
