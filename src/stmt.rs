use std::collections::HashMap;
use crate::{expr::{Expr}, lexer::{self, Lexer}};

pub struct Block {
    stmts: Vec<Expr>,
    vars: HashMap<String, Expr>,
}

struct FunSign {
    name: String,
    args: Vec<Expr>,
}

pub struct FunDefStmt {
    name: String,
    args: Vec<Expr>,
    return_exrp: Expr,
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
    BlockStmt(Block),
    FunDef(FunDefStmt),
    If(IfStmt),
    While(WhileStmt),
    CF(CFStmt),
}


fn parse_fun_def(lexer: &mut Lexer) -> Option<Stmt> {
    lexer.assert_token_kind(lexer::TOK_FN);
    lexer.eat(); // eat fn
    lexer.assert_token_kind(lexer::TOK_IDEN);
    let name: String = lexer.front().get_str_val()
        .unwrap().to_string();
    lexer.eat(); // eat name
    
    None   
}
