use std::{fs::File, io::Write, collections::HashMap};
use crate::fe::stmt::{Stmt, CFStmt, FunDefStmt, VarAssignStmt, Type, Block};
use crate::fe::expr::{Expr, CallExpr, BinExpr};
use crate::compiler_felf64::native_api::{is_native_api, compile_api};

pub struct Asm {
    pub glob: Vec<String>,
    pub text: Vec<String>,
    pub data: Vec<String>,
    counter: u32,
}

pub struct Var {
    pub bp_offset: u32,
    pub size: u32,
}

pub struct Context {
    pub cur_bp_offset: u32,
    pub vars: HashMap<String, Var>,
}

impl Var {
    fn new(bp_offset: u32, size: u32) -> Self {
        Var{bp_offset: bp_offset, size: size}
    }
}

impl Context {
    fn new() -> Self {
        Context{cur_bp_offset: 4, vars: HashMap::new()}
    }
    fn inc_bp_offset(&mut self, size: u32) {
        self.cur_bp_offset += size;
    }
}

impl Asm {
    fn new() -> Self {Asm{glob: vec![], text: vec![], data: vec![], counter: 0}}
    pub fn get_and_inc(&mut self) -> u32 {
        let ret = self.counter;
        self.counter += 1;
        ret
    }
}

fn compile_call_expr(call: &CallExpr, asm: &mut Asm, ctx: &mut Context) {
    if is_native_api(&call.name) {compile_api(&call, asm, ctx);}
}

fn compile_var_assign(var_assign: &VarAssignStmt, asm: &mut Asm, ctx: &mut Context) {
    match var_assign.expr {
        Expr::Int(num) => {
            let offset = ctx.cur_bp_offset;
            asm.text.push(format!("\tmov DWORD [rbp-{}], {}", offset, num));
            ctx.vars.insert(var_assign.name.clone(), Var::new(offset, 4));
            ctx.inc_bp_offset(4);
        },
        _ => {}
    }
}

fn compile_expr(expr: &Expr, asm: &mut Asm, ctx: &mut Context) {
    match expr {
	    Expr::Call(x) => compile_call_expr(x, asm, ctx),
	    _ => todo!("Not yet implemented"),
    };
}

fn compile(stmt: &Stmt, asm: &mut Asm, ctx: &mut Context) {
    match stmt {
        Stmt::VarAssign(x) => compile_var_assign(x, asm, ctx),
        Stmt::ExprStmt(x) => compile_expr(x, asm, ctx),
	    _ => todo!("Not yet implemented"),
    };
}

pub fn compiler(filename: String, stmts: &Vec<Stmt>) {   
    let mut asm: Asm = Asm::new();
    asm.glob.push(String::from("global _start"));
    asm.data.push(String::from("section .data"));
    asm.text.extend(vec![
        "section .text".to_string(),
        "_start:".to_string(),
        "\tpush rbp".to_string(),
        "\tmov rbp, rsp".to_string(),
    ]);
    let mut ctx: Context = Context::new();
    for stmt in stmts.iter() {compile(stmt, &mut asm, &mut ctx);}
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
