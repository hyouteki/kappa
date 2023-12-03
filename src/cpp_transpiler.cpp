#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include "parser.hpp"
#include "cpp_transpiler.hpp"

void save(const std::string);
void include_string();
std::string get_type(const Expr_Kind);
std::string type_with_name(const Var);
void transpile_fun_def(const Stmt);
void transpile_if(const Stmt);
void transpile_expr(const Stmt);
void transpile_var(const Stmt);
void transpile_return(const Stmt);
void transpile_while(const Stmt);
void transpile_stmt(const Stmt);

bool string_included = false;
bool vecter_included = false;
std::vector<std::string> headers;
std::vector<std::string> lines;

void cpp_transpile(const std::string filename, const std::vector<Stmt> stmts) {
	(void)stmts;
	headers.push_back("#include <stdio.h>");
	headers.push_back("#include <stdio.h>");
	for (Stmt stmt: stmts) transpile_stmt(stmt);
	save(filename);
}

void save(const std::string filename) {
	std::ofstream file(filename);
	for (std::string line: headers) file << line << std::endl;
	for (std::string line: lines) file << line << std::endl;
	file.close();
}

void include_string() {
	if (!string_included) headers.push_back("#include <string>");
	string_included = true;
}

std::string get_type(const Expr_Kind kind) {
	switch (kind) {
		case STR:
			include_string();
			return "std::string";
		case BOOL:
			return "bool";
		case INT:
			return "int";
		case ANY:
			return "auto";
			break;
		case NULL_EXPR:
			return "void";
			break;
		case FUN_CALL:
			return "auto";
		case _VAR: 
		case _RE_ASS:
		case _MIX: 
		case _WF:;
	}
	return "";
} 

std::string type_with_name(const Var var) {
	std::string line = "";
	if (!var.mut) line += "const ";
	line += get_type(var.type);
	line.push_back(' ');
	line += var.name;
	return line;
} 

void transpile_fun_def(const Stmt stmt) {
	Fun fun = stmt.fun();
	std::string line = "";
	line += get_type(fun.return_type);
	line.push_back(' ');
	line += fun.name;
	line.push_back('(');
	lines.push_back(line);
}

void transpile_if(const Stmt stmt) {}
void transpile_expr(const Stmt stmt) {}
void transpile_var(const Stmt stmt) {}
void transpile_return(const Stmt stmt) {}
void transpile_while(const Stmt stmt) {}

void transpile_stmt(const Stmt stmt) {
	switch(stmt.kind) {
		case Stmt::FUN_DEF:
			transpile_fun_def(stmt);	 
			break;
        case Stmt::IF:
			transpile_if(stmt);
			break;
        case Stmt::EXPR:
			transpile_expr(stmt);	 
			break;
        case Stmt::VAR:
			transpile_var(stmt);	 
			break;
        case Stmt::RETURN:
			transpile_return(stmt);	 
			break;
        case Stmt::WHILE:
			transpile_while(stmt);
			break;			
	}
}
