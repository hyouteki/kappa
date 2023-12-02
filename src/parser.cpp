#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <cassert>
#include "lexer.hpp"
#include "parser.hpp"
#include "simulator.hpp"

std::unordered_set<std::string> reserved_strs = {
    "fun", "if", "else", "true", "false", "var", "int", "while", "return",
    "val", "bool", "str", "null", "break", "continue"
};

std::unordered_set<Lexeme_Kind> operators = {
    PLUS,
    MINUS,
    MUL,
    DIV,
    MOD,
    POW,
    AND,
    BIT_AND,
    OR,
    BIT_OR,
    BIT_XOR,
    COMP_EQUAL,
    COMP_NOT_EQUAL,
    COMP_LT,
    COMP_GT,
    COMP_LT_EQUAL,
    COMP_GT_EQUAL,
};

Expr::Expr(int num) {
    this->kind = INT;
    this->val.int_val = num;
}

Expr::Expr(bool arg) {
    this->kind = BOOL;
    this->val.bool_val = arg;
}

Expr::Expr(std::string str) {
    this->kind = STR;
    this->val.str_val = str;
}

std::string Expr::str_val() const {
    if (this->kind != STR && this->kind != _VAR && this->kind != _WF) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected Expr_Type `STR`; got '";
        std::cerr << this->kind << "'" << std::endl;
        exit(1);
    }
    return this->val.str_val;
}

bool Expr::bool_val() const {
    if (this->kind != BOOL) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected Expr_Type `BOOL`; got '";
        std::cerr << this->kind << "'" << std::endl;
        exit(1);
    }
    return this->val.bool_val;
}

Fun_Call Expr::fun_call() const {
    if (this->kind != FUN_CALL) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected Expr_Type `FUN_CALL`; got '";
        std::cerr << this->kind << "'" << std::endl;
        exit(1);
    }
    return this->val.fun_call;
}

void While::print() const {
	std::cout << "while (";
	this->condition.print();
	std::cout << ") {" << std::endl;
	for (Stmt stmt: this->block) {
		std::cout << "\t";
		stmt.print();
		std::cout << std::endl;
	}
	std::cout << "}";
}

int Expr::int_val() const {
    if (this->kind != INT) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected Expr_Type `INT`; got '";
        std::cerr << this->kind << "'" << std::endl;
        exit(1);
    }
    return this->val.int_val;
}

std::string Expr::str() const {
    switch (this->kind) {
        case ANY:
        case STR: return this->str_val();
        case BOOL: return (this->bool_val())? "true": "false";
        case INT: return std::to_string(this->int_val());
        case NULL_EXPR: return "null";
        default:
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":";
            std::cerr << __LINE__ << std::endl << "ERROR: Cannot stringify Expr_Kind '";
            std::cerr << this->kind << "'" << std::endl;
            exit(1);
    }
    return "";
}

void Expr::print() const {
    switch (this->kind) {
        case ANY:
        case STR:
            std::cout << this->str_val();
            return;
        case INT:
            std::cout << this->int_val();
            return;
        case NULL_EXPR:
            std::cout << "null";
            return;
        case BOOL:
            if (this->bool_val()) std::cout << "true";
            else std::cout << "false";
            return;
        case _VAR:
            std::cout << this->str_val();
            return;
        case FUN_CALL:
            this->val.fun_call.print();
            return;
        case _MIX:
            assert(this->val1);
            assert(this->val2);
            this->val1->print();
            std::cout << " " << lexeme_kind_to_str(this->op) << " ";
            this->val2->print();
            return;
		case _RE_ASS:
			std::cout << this->str_val() << " = ";
			return;
		case _WF:
			std::cout << this->str_val();
			return;
    }
	std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
    std::cerr << "ERROR: Invalid Expr_Kind" << std::endl;
    exit(1);
}

If::If(Lexer* lexer) {
    lexer->assert_lexeme_front("if");
    lexer->del_front();
    lexer->assert_lexeme_front(OPEN_PAREN);
    lexer->del_front();
    std::optional<Expr> out = parse_expr(lexer);
    if (!out) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << lexer->filename << ":"; lexer->front().loc.print();
        std::cerr << ": ERROR: If condition has not been provided" << std::endl;
        exit(1);
    }
    this->condition = *out;
    lexer->assert_lexeme_front(CLOSE_PAREN);
    lexer->del_front();
    lexer->assert_lexeme_front(OPEN_CURLY);
    lexer->del_front();
    while (!lexer->front().equal(CLOSE_CURLY)) {
        std::optional<Stmt> out = iter_lexer(lexer);
        if (out) this->then_block.push_back(*out);
    }
    lexer->del_front();
    if (lexer->is_lexeme_front("else")) {
        lexer->del_front();
        lexer->assert_lexeme_front(OPEN_CURLY);
        lexer->del_front();
        while (!lexer->front().equal(CLOSE_CURLY)) {
            std::optional<Stmt> out = iter_lexer(lexer);
            if (out) this->else_block.push_back(*out);
        }
        lexer->del_front();
    }
}

void If::print() const {
    std::cout << "if (";
    this->condition.print();
    std::cout << ") {" << std::endl;
    for (size_t i = 0; i < this->then_block.size(); ++i) {
		std::cout << "\t";
		this->then_block[i].print();
		std::cout << std::endl;
    }
    std::cout << "}";
    if (this->else_block.empty()) return;
    std::cout << " else {" << std::endl;
    for (size_t i = 0; i < this->else_block.size(); ++i) {
		std::cout << "\t";
		this->else_block[i].print();
        std::cout << std::endl;
    }
    std::cout << "}";
}

While::While(Lexer* lexer) {
    lexer->assert_lexeme_front("while");
    lexer->del_front();
    lexer->assert_lexeme_front(OPEN_PAREN);
    lexer->del_front();
    std::optional<Expr> out = parse_expr(lexer);
    if (!out) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << lexer->filename << ":"; lexer->front().loc.print();
        std::cerr << ": ERROR: While condition has not been provided" << std::endl;
        exit(1);
    }
    this->condition = *out;
    lexer->assert_lexeme_front(CLOSE_PAREN);
    lexer->del_front();
    lexer->assert_lexeme_front(OPEN_CURLY);
    lexer->del_front();
    while (!lexer->front().equal(CLOSE_CURLY)) {
        std::optional<Stmt> out = iter_lexer(lexer);
        if (out) this->block.push_back(*out);
    }
    lexer->del_front();
}

Stmt::Stmt(const Fun fun) {
    this->kind = FUN_DEF;
    this->val.fun = fun;
}

Stmt::Stmt(const If if_cond) {
    this->kind = IF;
    this->val.if_cond = if_cond;
}

Stmt::Stmt(const Expr expr) {
    this->kind = EXPR;
    this->val.expr = expr;
}

Stmt::Stmt(const Var var) {
    this->kind = VAR;
    this->val.var = var;
}

Stmt::Stmt(const While while_block) {
    this->kind = WHILE;
    this->val.while_block = while_block;
}

Fun Stmt::fun() const {
    if (this->kind != FUN_DEF) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected Expr_Type `FUN_DEF`; got '";
        std::cerr << stmt_kind_to_str(this->kind) << "'" << std::endl;
        exit(1);
    }
    return this->val.fun;
}
If Stmt::if_cond() const {
    if (this->kind != IF) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected Expr_Type `IF`; got '";
        std::cerr << stmt_kind_to_str(this->kind) << "'" << std::endl;
        exit(1);
    }
    return this->val.if_cond;
}
Expr Stmt::expr() const {
    if (this->kind != EXPR && this->kind != RETURN) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected Expr_Type `EXPR`; got '";
        std::cerr << stmt_kind_to_str(this->kind) << "'" << std::endl;
        exit(1);
    }
    return this->val.expr;
}
Var Stmt::var() const {
    if (this->kind != VAR) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected Expr_Type `VAR`; got '";
        std::cerr << stmt_kind_to_str(this->kind) << "'" << std::endl;
        exit(1);
    }
    return this->val.var;
}

While Stmt::while_block() const {
    if (this->kind != WHILE) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << "ERROR: Expected Expr_Type `WHILE`; got '";
        std::cerr << stmt_kind_to_str(this->kind) << "'" << std::endl;
        exit(1);
    }
    return this->val.while_block;
}

void Stmt::print() const {
    switch (this->kind) {
        case FUN_DEF:
            this->fun().print();
            break;
        case EXPR:
            this->expr().print();
            break;
        case IF:
            this->if_cond().print();
            break;
        case RETURN:
            std::cout << "return "; this->expr().print();
            break;
		case WHILE:
			this->while_block().print();
			break;
        case VAR:
            std::cout << this->var().name << " = "; (*this->var().expr).print();
            break;
        default:
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
			std::cerr << "ERROR: Invalid Stmt_Kind '" << stmt_kind_to_str(this->kind);
			std::cerr << "'" << std::endl;
            exit(1);
    }
}

std::optional<Expr> parse_expr(Lexer* lexer) {
    Expr* expr = new Expr();
    if (is_fun_call(lexer)) {
        Fun_Call fun_call = Fun_Call(lexer);
        expr->kind = FUN_CALL;
        expr->val.fun_call = fun_call;
    } else if (lexer->front().equal(STR_LIT)) {
        expr->kind = STR;
        expr->val.str_val = lexer->front().str.substr(1,
            lexer->front().str.size()-2);
        lexer->del_front();
    } else if (lexer->front().is_int()) {
        expr->kind = INT;
        expr->val.int_val = std::stoi(lexer->front().str);
        lexer->del_front();
    } else if (!lexer->is_lexeme_front(NAME)) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << lexer->filename << ":"; lexer->front().loc.print();
        std::cerr << ": ERROR: Invalid expression '" << lexer->front().str << "'" << std::endl;
        exit(1);
    } else if (lexer->front().equal("true")) {
        expr->kind = BOOL;
        expr->val.bool_val = true;
        lexer->del_front();
    } else if (lexer->front().equal("false")) {
        expr->kind = BOOL;
        expr->val.bool_val = false;
        lexer->del_front();
    } else if (lexer->front().equal("null")) {
        expr->kind = NULL_EXPR;
        lexer->del_front();
    } else {
        expr->kind = _VAR;
        expr->val.str_val = lexer->front().str;
        lexer->del_front();
    }
    if (lexer->is_lexeme_front(operators)) {
        expr->op = lexer->front().kind;
        lexer->del_front();
        expr->val1 = new Expr();
        expr->val1->kind = expr->kind;
        expr->val1->val = expr->val;
        expr->kind = _MIX;
        expr->val2 = new Expr(*parse_expr(lexer));
    }
    return *expr;
}

Fun_Call::Fun_Call(Lexer* lexer) {
    this->name = lexer->front().str;
    lexer->del_front();
    lexer->del_front();
    while (!lexer->front().equal(CLOSE_PAREN)) {
        this->args.push_back(*parse_expr(lexer));
        if (lexer->front().equal(CLOSE_PAREN)) continue;
        lexer->assert_lexeme_front(COMMA);
        lexer->del_front();
    }
    lexer->del_front();
}

void Fun_Call::print() const {
    std::cout << this->name << "(";
    for (size_t i = 0; i < this->args.size(); ++i) {
        this->args[i].print();
        if (i < this->args.size()-1) std::cout << ", ";
    }
    std::cout << ")";
}

void Fun::print() const {
    std::cout << "fun " << this->name << "(";
    for (size_t i = 0; i < this->args.size(); ++i) {
        std::cout << this->args[i].name;
        if (i < this->args.size()-1) std::cout << ", ";
    }
    std::cout << ") {" << std::endl;
	for (Stmt stmt: this->block) {
		std::cout << "\t";
		stmt.print();
		std::cout << std::endl;
	}
	std::cout << "}";
}

bool is_fun_call(const Lexer* lexer) {
    std::vector<Lexeme_Kind> regex = {NAME, OPEN_PAREN};
    return (lexer->is_lexeme_front(regex) &&
        reserved_strs.find(lexer->front().str) == reserved_strs.end());
}

std::optional<Stmt> iter_lexer(Lexer* lexer) {
    if (lexer->front().equal(SEMI)) {
        lexer->del_front();
        return {};
    }
    else if (lexer->is_lexeme_front("fun")) return *(new Stmt(Fun(lexer)));
    else if (lexer->is_lexeme_front("if")) return *(new Stmt(If(lexer)));
    else if (lexer->is_lexeme_front("while")) return *(new Stmt(While(lexer)));
	else if (lexer->is_lexeme_front("return")) return parse_return_stmt(lexer); 
    else if (lexer->front().equal("var") || lexer->front().equal("val"))
        return *(new Stmt(assign_var(lexer)));
    std::optional<Stmt> out = parse_expr_stmt(lexer);
    if (out) return *out;
    std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
    std::cerr << lexer->filename << ":"; lexer->front().loc.print();
    std::cerr << ": ERROR: Invalid token '" << lexer->front().str << "'" << std::endl;
    exit(1);
}

Fun::Fun(Lexer *lexer) {
    lexer->assert_lexeme_front("fun");
    lexer->del_front();
    lexer->assert_lexeme_front(NAME);
    this->name = lexer->front().str;
    lexer->del_front();
    lexer->assert_lexeme_front(OPEN_PAREN);
    lexer->del_front();
    while (!lexer->is_lexeme_front(CLOSE_PAREN)) {
        Var* var = new Var();
        lexer->assert_lexeme_front(NAME);
        var->name = lexer->front().str;
        lexer->del_front();
        lexer->assert_lexeme_front(COLON);
        lexer->del_front();
        lexer->assert_lexeme_front(NAME);
        var->type = str_to_expr_kind(lexer);
        lexer->del_front();
        if (lexer->is_lexeme_front(EQUAL)) {
            lexer->del_front();
            Expr expr = *parse_expr(lexer);
            if (var->type != expr.kind) {
                std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
                std::cerr << lexer->filename << ":"; lexer->front().loc.print();
                std::cerr << ": ERROR: Type mismatch, expected type '" << expr_kind_to_str(var->type);
                std::cerr << "'; got '" << expr_kind_to_str(expr.kind) << "'" << std::endl;
                exit(1);
            }
            var->expr = expr;
        }
        this->args.push_back(*var);
        if (lexer->front().equal(CLOSE_PAREN)) continue;
        lexer->assert_lexeme_front(COMMA);
        lexer->del_front();
    }
    lexer->del_front();
    lexer->assert_lexeme_front(COLON);
    lexer->del_front();
    lexer->assert_lexeme_front(NAME);
    this->return_type = str_to_expr_kind(lexer);
    lexer->del_front();
    lexer->assert_lexeme_front(OPEN_CURLY);
    lexer->del_front();
    while (!lexer->is_lexeme_front(CLOSE_CURLY)) {
        std::optional<Stmt> out = iter_lexer(lexer);
        if (out) this->block.push_back(*out);
    }
    lexer->del_front();
}

std::vector<Stmt> parse_lexer(Lexer *lexer) {
    std::vector<Stmt> block;
    while (!lexer->empty()) {
        std::optional<Stmt> out = iter_lexer(lexer);
        if (out) block.push_back(*out);
    }
    return block;
}

Var assign_var(Lexer* lexer) {
    Var var;
    if (lexer->is_lexeme_front("var")) var.mut = true;
    else if (lexer->is_lexeme_front("val")) var.mut = false;
    lexer->del_front();
    lexer->assert_lexeme_front(NAME);
    var.name = lexer->front().str;
    lexer->del_front();
    lexer->assert_lexeme_front(COLON);
    lexer->del_front();
    lexer->assert_lexeme_front(NAME);
    var.type = str_to_expr_kind(lexer);
    lexer->del_front();
    lexer->assert_lexeme_front(EQUAL);
    lexer->del_front();
    Expr expr = *parse_expr(lexer);
    var.expr = expr;
    return var;
}

std::string expr_kind_to_str(const Expr_Kind kind) {
    switch (kind) {
        case STR: return "str";
        case INT: return "int";
        case BOOL: return "bool";
        case ANY: return "any";
        case FUN_CALL: return "fun_call";
        case NULL_EXPR: return "null";
        case _VAR: return "var";
        case _MIX: return "_MIX";
        default:
            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
            std::cerr << "ERROR: Invalid Expr_Kind " << kind << "'" << std::endl;
            exit(1);
    }
}

std::string stmt_kind_to_str(const Stmt::Stmt_Kind kind) {
    switch (kind) {
        case Stmt::FUN_DEF: return "FUN_DEF";
        case Stmt::IF: return "IF";
        case Stmt::EXPR: return "EXPR";
        case Stmt::VAR: return "VAR";
        case Stmt::RETURN: return "RETURN";
		case Stmt::WHILE: return "WHILE";
	} 
   	std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
   	std::cerr << "ERROR: Invalid Stmt_Kind " << kind << "'" << std::endl;
   	exit(1);
    return "";
}

Expr_Kind str_to_expr_kind(Lexer* lexer) {
    if (lexer->front().str == "str") return STR;
    else if (lexer->front().str == "int") return INT;
    else if (lexer->front().str == "bool") return BOOL;
    else if (lexer->front().str == "any") return ANY;
    else if (lexer->front().str == "null") return NULL_EXPR;
    else {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << lexer->filename << ":"; lexer->front().loc.print();
        std::cerr << ": ERROR: Invalid datatype '" << lexer->front().str << "'" << std::endl;
        exit(1);
    }
    return NULL_EXPR;
}

std::optional<Stmt> parse_expr_stmt(Lexer* lexer) {
    std::optional<Expr> out = parse_expr(lexer);
    if (!out) return {};
    Expr expr = *out;
    if (expr.kind != _VAR) return *(new Stmt(expr));
    std::string name = expr.str_val();
    if (name == "break" || name == "continue") {
        Expr* tmp = new Expr();
        tmp->kind = _WF;
        tmp->val.str_val = name;
        return *(new Stmt(*tmp));
    }
    lexer->assert_lexeme_front(EQUAL);
    lexer->del_front();
    out = parse_expr(lexer);
    if (out) {
        Expr tmp = *out;
        Var* var = new Var();
        var->name = name;
        var->type = _RE_ASS;
        var->expr = tmp;
        return *(new Stmt(*var));
    }
    return {};
}

bool are_expr_kinds_compatible(const Expr_Kind var_kind, const Expr_Kind expr_kind) {
    if (var_kind == ANY) return true;
    if (expr_kind == NULL_EXPR) return true;
    return var_kind == expr_kind;
}

Expr assert_null_check(Lexer* lexer, std::optional<Expr> expr) {
    if (expr) {
        std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::cerr << lexer->filename << ":"; lexer->front().loc.print();
        std::cerr << ": ERROR: Expected an expression, got nothing" << std::endl;
        exit(1);
    }
    return *expr;
}

Stmt parse_return_stmt(Lexer* lexer) {
	lexer->del_front();
    Stmt* stmt = new Stmt(*parse_expr(lexer));
    stmt->kind = Stmt::RETURN;
	return *stmt;
}
