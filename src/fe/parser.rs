use crate::fe::{lexer::Lexer, stmt::{Stmt, parse_stmt}};

pub fn parse_lexer(lexer: &mut Lexer) -> Vec<Stmt> {
    let mut stmts: Vec<Stmt> = Vec::new();
    while !lexer.empty() {
        match parse_stmt(lexer) {
            Some(x) => stmts.push(x),
            None => {},
        }
    }
    stmts
}
