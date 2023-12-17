use std::collections::HashMap;
use crate::{expr::{Expr}, lexer::{self, Lexer}};

pub struct Block {
    stmts: Vec<Stmt>,
    vars: HashMap<String, Expr>,
}

pub enum Type {
    Str,
    Int,
    Bool,
}

pub struct Arg {
    name: String,
    arg_type: Type, 
}

pub struct FunDefStmt {
    name: String,
    args: Vec<Arg>,
    return_expr: Type,
    block: Block,
}

pub struct IfStmt {
    condition: Expr,
    then_block: Block,
    else_block: Block,
}

pub struct WhileStmt {
    condition: Expr,
    block: Block,
}

pub enum CFStmt {
    Return(Expr),
    Break,
    Continue,
}

pub enum Stmt {
    BlockStmt(Box<Block>),
    FunDef(FunDefStmt),
    If(IfStmt),
    While(WhileStmt),
    CF(CFStmt),
}

impl Block {
    fn new(stmts: Vec<Stmt>) -> Self {
        Block{stmts: stmts, vars: HashMap::new()}
    }
}

impl FunDefStmt {
    fn new(name: String, args: Vec<Arg>, 
        return_expr: Type, block: Block) -> Self {
        FunDefStmt{name: name, args: args, 
            return_expr: return_expr, block: block}
    }
}

fn token_kind_to_type(kind: i32) -> Option<Type> {
    match kind {
        lexer::TOK_TYPE_INT => Some(Type::Int),
        lexer::TOK_TYPE_STR => Some(Type::Str),
        lexer::TOK_TYPE_BOOL => Some(Type::Bool),
        _ => None
    }
}

fn parse_type(lexer: &mut Lexer) -> Type {
    let x: Option<Type> = token_kind_to_type(lexer.front().kind);
    if x.is_none() {lexer.error(String::from("invalid type"), None);}
    lexer.eat(); // eat type
    x.unwrap()
}

fn parse_block(lexer: &mut Lexer) -> Block {
    lexer.assert_token_kind('{' as i32);
    lexer.eat(); // eat '{'
    let mut stmts: Vec<Stmt> = Vec::new();
    while !lexer.empty() && !lexer.is_token_kind('}' as i32) {
        match parse_stmt(lexer) {
            Some(x) => stmts.push(x),
            None => {},
        }
        if !lexer.empty() && 
            !lexer.is_token_kind('}' as i32) {break;}     
    }    
    lexer.eat(); // eat '}'
    Block::new(stmts)
}

pub fn parse_stmt(lexer: &mut Lexer) -> Option<Stmt> {
    match lexer.front().kind {
        lexer::TOK_FN => parse_fun_def(lexer),
        _ => {
            lexer.error(String::from("invalid stmt"), None);
            None
        }
    }
}

fn parse_fun_def(lexer: &mut Lexer) -> Option<Stmt> {
    lexer.assert_token_kind(lexer::TOK_FN);
    lexer.eat(); // eat fn
    lexer.assert_token_kind(lexer::TOK_IDEN);
    let name: String = lexer.front()
        .get_str_val().unwrap().to_string();
    lexer.eat(); // eat name
    lexer.assert_token_kind('(' as i32);
    lexer.eat(); // eat '('
    let mut args: Vec<Arg> = Vec::new();
    while !lexer.empty() && !lexer.is_token_kind(')' as i32) {
        lexer.assert_token_kind(lexer::TOK_IDEN);
        let arg_name: String = lexer.front()
            .get_str_val().unwrap().to_string();
        lexer.eat(); // eat arg_name
        lexer.assert_token_kind(':' as i32);
        lexer.eat(); // eat ':'
        lexer.assert_token();
        let arg_type: Type = parse_type(lexer);
        args.push(Arg{name: arg_name, arg_type: arg_type});
        if !lexer.empty() && lexer.is_token_kind(')' as i32) {break;}
        lexer.assert_token_kind(',' as i32);
        lexer.eat(); // eat ','
    }    
    lexer.eat(); // eat ')'
    lexer.assert_token_kind(':' as i32);
    lexer.eat(); // eat ':'
    let return_type: Type = parse_type(lexer);
    let block: Block = parse_block(lexer);
    Some(Stmt::FunDef(FunDefStmt::new(name, args, return_type, block)))
}
