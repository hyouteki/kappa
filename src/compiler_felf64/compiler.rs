use std::{fs::File, io::Write};
use crate::fe::stmt::{Stmt, CFStmt, FunDefStmt, VarAssignStmt, Type, Block};
use crate::fe::expr::{Expr, CallExpr, BinExpr};
use crate::compiler_felf64::native_api::{is_native_api, compile_api};

pub struct Asm {
    pub glob: Vec<String>,
    pub text: Vec<String>,
    pub data: Vec<String>,
}

impl Asm {fn new() -> Self {Asm{glob: vec![], text: vec![], data: vec![]}}}

pub fn name_to_label(name: &String) -> String {format!("kappa_data_{}:", name)}

fn compile_call_expr(call: &CallExpr, asm: &mut Asm) {
    if is_native_api(&call.name) {compile_api(&call, asm);}
}

fn compile_expr(expr: &Expr, asm: &mut Asm) {
    match expr {
	Expr::Call(x) => compile_call_expr(x, asm),
	_ => todo!("Not yet implemented"),
    };
}

fn compile(stmt: &Stmt, asm: &mut Asm) {
    match stmt {
	Stmt::ExprStmt(x) => compile_expr(x, asm),
	_ => todo!("Not yet implemented"),
    };
}

pub fn compiler(filename: String, stmts: &Vec<Stmt>) {   
    let mut asm: Asm = Asm::new();
    asm.glob.push(String::from("global _start"));
    asm.text.push(String::from("section .text"));
    asm.text.push(String::from("_start:"));
    for stmt in stmts.iter() {compile(stmt, &mut asm);}
    {
	let mut file = File::create(filename)
	    .expect("could not create a file");
	for line in asm.glob.iter() {
            file.write_all(line.as_bytes()).expect("could not write line");
            file.write_all(b"\n").expect("could not write new line");
	}
	for line in asm.text.iter() {
            file.write_all(line.as_bytes()).expect("could not write line");
            file.write_all(b"\n").expect("could not write new line");
	}
	for line in asm.data.iter() {
            file.write_all(line.as_bytes()).expect("could not write line");
            file.write_all(b"\n").expect("could not write new line");
	}
    }
}
