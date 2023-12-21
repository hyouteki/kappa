use std::{fs::File, io::Write};
use crate::fe::stmt::{Stmt, CFStmt, FunDefStmt, VarAssignStmt, Type, Block};
use crate::fe::expr::{Expr, CallExpr, BinExpr};

pub fn compiler(filename: String, stmts: Vec<Stmt>) {   
    let mut lines: Vec<String> = Vec::new();
    lines.push(String::from("global _start"));
    for stmt in stmts.iter() {compile(stmt, &mut lines);}
    let mut file = File::create(filename).expect("could not create a file");
    for line in lines.iter() {
        file.write_all(line.as_bytes()).expect("could not write line");
        file.write_all(b"\n").expect("could not write new line");
    }
}
