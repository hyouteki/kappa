use std::{fs::File, io::Write, collections::HashMap};
use crate::fe::stmt::{Stmt, CFStmt, VarAssignStmt, VarMutStmt, IfStmt, WhileStmt, Type, Block};
use crate::fe::expr::{Expr, CallExpr};
use crate::fe::lexer::{TOK_GT, TOK_LT, TOK_GE, TOK_LE, TOK_EQ, TOK_NE};
use crate::compiler_felf64::native_api::{is_native_api, compile_api};
use crate::utils::{error, strlen};

pub struct Asm {
    pub glob: Vec<String>,
    pub text: Vec<String>,
    pub data: Vec<String>,
    counter: u32,
}

#[derive(Clone)]
pub struct Var {
    pub bp_offset: u32,
    pub size: u32,
    pub var_type: Type, 
}

#[derive(Clone)]
pub struct Context {
    pub break_label: String,
    pub continue_label: String,
    pub cur_bp_offset: u32,
    pub vars: HashMap<String, Var>,
}

impl Var {
    fn new(bp_offset: u32, size: u32, var_type: Type) -> Self {
        Var{bp_offset: bp_offset, size: size, var_type: var_type}
    }
}

impl Context {
    fn new() -> Self {
        Context{break_label: "".to_string(), continue_label: "".to_string(),
                cur_bp_offset: 0, vars: HashMap::new()}
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
    pub fn push(&mut self, data: &String) {
        if data.starts_with("DWORD") {
            self.text.push(format!("\tmov ecx, {}", data));
        } else {self.text.push(format!("\tmov rcx, {}", data));}
        self.text.push("\tpush rcx".to_string());
    }
    pub fn mov_to_reg(&mut self, reg: &String, data: &String) {
        self.text.push(format!("\tmov {}{}, {}",
                               {if data.starts_with("DWORD") {"e"} else {"r"}}.to_string(), reg, data));
    }
    pub fn mov_from_reg(&mut self, data: &String, reg: &String) {
        self.text.push(format!("\tmov {}, {}{}", data,
                               {if data.starts_with("DWORD") {"e"} else {"r"}}.to_string(), reg));
    }
    pub fn is_reg(data: &String) -> bool {
        vec!["rax", "rbx", "rcx", "eax", "ebx", "ecx"].contains(&data.as_str())
    }
    pub fn mov(&mut self, to: &String, from: &String) {
        let mut flag: bool = true;
        if Asm::is_reg(from) {flag = false; self.mov_from_reg(to, &from[1..].to_string());}
        if Asm::is_reg(to) {flag = false; self.mov_to_reg(&to[1..].to_string(), from);}
        if flag {self.text.push(format!("\tmov {}, {}", to, from));}
    }
}

fn compile_call_expr(call: &CallExpr, asm: &mut Asm, ctx: &mut Context) {
    if is_native_api(&call.name) {compile_api(&call, asm, ctx);}
}

fn compile_var_assign(var_assign: &VarAssignStmt, asm: &mut Asm, ctx: &mut Context) {
    match &var_assign.expr {
        Expr::Int(num) => {
            let size = 4;
            let offset = ctx.cur_bp_offset + size;
            asm.text.push(format!("\tmov DWORD [rbp-{}], {}", offset, num));
            ctx.vars.insert(var_assign.name.clone(), Var::new(offset, size, Type::Int));
            ctx.inc_bp_offset(size);
        },
        Expr::Bool(val) => {
            let size = 1;
            let offset = ctx.cur_bp_offset + size;
            asm.text.push(format!("\tmov BYTE [rbp-{}], {}", offset, val));
            ctx.vars.insert(var_assign.name.clone(), Var::new(offset, size, Type::Bool));
            ctx.inc_bp_offset(size);
        },
        Expr::Str(text) => {
            // storing the string length
            let len_size = 4;
            let len_offset = ctx.cur_bp_offset + len_size;
            asm.text.push(format!("\tmov DWORD [rbp-{}], {}", len_offset, strlen(text)+1));
            ctx.inc_bp_offset(len_size);
            // storing the string
            let size = 8;
            let offset = ctx.cur_bp_offset + size;
            let label_count = asm.get_and_inc();
            asm.text.push(format!("\tmov QWORD [rbp-{}], L{}", offset, label_count));
            asm.data.extend(vec![
                format!("L{}:", label_count),
                format!("\tdb `{}`, 0", text)
            ]);
            ctx.vars.insert(var_assign.name.clone(), Var::new(offset, size, Type::Str));
            ctx.inc_bp_offset(size);
        },
        Expr::Bin(_) => {
            let _ = access_expr_val(&var_assign.expr, asm, ctx);
            // TODO: assuming that bin_expr will result in only int
            let size = 4;
            let offset = ctx.cur_bp_offset + size; 
            asm.text.push(format!("\tmov DWORD [rbp-{}], eax", offset));
            ctx.vars.insert(var_assign.name.clone(), Var::new(offset, size, Type::Int));
            ctx.inc_bp_offset(size);
        }
        _ => todo!("yet to be implemented")
    }
}

fn compile_expr(expr: &Expr, asm: &mut Asm, ctx: &mut Context) {
    match expr {
	    Expr::Call(x) => compile_call_expr(x, asm, ctx),
	    _ => {/*any other expression at top level domain does not make sense to compute*/},
    };
}

fn compile_var_mut(var_mut: &VarMutStmt, asm: &mut Asm, ctx: &mut Context) {
    let var_eval = access_expr_val(&Expr::Var(var_mut.name.clone()), asm, ctx);
    let expr_eval = access_expr_val(&var_mut.expr, asm, ctx);
    asm.mov(&var_eval, &expr_eval);
}

fn compile_block(block: &Block, asm: &mut Asm, ctx: &mut Context) {
    for stmt in block.stmts.iter() {compile(stmt, asm, ctx);}
}

pub fn access_expr_val(expr: &Expr, asm: &mut Asm, ctx: &mut Context) -> String {
    match expr {
        Expr::Int(num) => num.to_string(),
        Expr::Str(text) => {
            let label_count = asm.get_and_inc();
            asm.data.extend(vec![
                format!("L{}:", label_count),
                format!("\tdb `{}`, 0", text),
            ]);
            format!("L{}", label_count)
        },
        Expr::Bool(val) => if *val {"1".to_string()} else {"0".to_string()},
        Expr::Var(name) => {
            let var = ctx.vars.get(name).unwrap();
            match var.var_type {
                Type::Int => format!("DWORD [rbp-{}]", var.bp_offset),
                Type::Bool => format!("BYTE [rbp-{}]", var.bp_offset),
                Type::Str => format!("QWORD [rbp-{}]", var.bp_offset),
            }
        },
        Expr::Bin(bin_expr) => {
            let lhs = access_expr_val(&bin_expr.lhs, asm, ctx);
            asm.push(&lhs);
            let rhs = access_expr_val(&bin_expr.rhs, asm, ctx);
            asm.push(&rhs);
            if bin_expr.op < 0 {todo!("TODO: access_expr_val::Bin::Op < 0 (Named Operators)")}
            asm.text.extend(vec![
                "\tpop rbx".to_string(),
                "\tpop rax".to_string(),
            ]);
            let op_char = char::from_u32(bin_expr.op.try_into().unwrap()).unwrap();
            asm.text.push(match op_char {
                '+' => "\tadd rax, rbx",
                '-' => "\tsub rax, rbx",
                '*' => "\tmul rbx",
                '/' => "\tdiv rbx",
                _ => {
                    error(format!("unsupported operator {}", op_char));
                    unreachable!()
                }
            }.to_string());
            "rax".to_string()
        }
        _ => todo!("TODO: access_expr_val"),
    }
}

fn compare_exprs(expr1: &Expr, expr2: &Expr, asm: &mut Asm, ctx: &mut Context) {
    let lhs: String = access_expr_val(expr1, asm, ctx);
    asm.mov_to_reg(&"bx".to_string(), &lhs);
    let rhs: String = access_expr_val(expr2, asm, ctx);
    asm.text.push(format!("\tcmp rbx, {}", rhs));
}

fn compile_if(if_stmt: &IfStmt, asm: &mut Asm, ctx: &mut Context) {
    let mut label_count = asm.get_and_inc();
    let else_label = format!("L{}", label_count);
    label_count = asm.get_and_inc();
    let final_label = format!("L{}", label_count);
    match &if_stmt.condition {
        Expr::Bool(val) => {
            if *val {
                let mut then_block_ctx = ctx.clone();
                compile_block(&if_stmt.then_block, asm, &mut then_block_ctx);
                asm.text.push(format!("\tjmp {}", final_label));
            } else {
                let mut else_block_ctx = ctx.clone();
                asm.text.push(format!("{}:", else_label));
                compile_block(&if_stmt.else_block, asm, &mut else_block_ctx);
                asm.text.push(format!("\tjmp {}", final_label));
            }
        },
        Expr::Bin(x) => {
            compare_exprs(&x.lhs, &x.rhs, asm, ctx);
            let cond_jmp: String = match x.op {
                TOK_GT => "jle",
                TOK_LT => "jge",
                TOK_GE => "jl",
                TOK_LE => "jg",
                TOK_EQ => "jne",
                TOK_NE => "je",
                _ => unreachable!(),
            }.to_string();
            asm.text.push(format!("\t{} {}", cond_jmp, else_label));
            let mut then_block_ctx = ctx.clone();
            compile_block(&if_stmt.then_block, asm, &mut then_block_ctx);
            asm.text.extend(vec![
                format!("\tjmp {}", final_label),
                format!("{}:", else_label),
            ]);
            let mut else_block_ctx = ctx.clone();
            compile_block(&if_stmt.else_block, asm, &mut else_block_ctx);
            asm.text.push(format!("\tjmp {}", final_label));
        }
        _ => unreachable!(),
    };
    asm.text.push(format!("{}:", final_label));
}

fn compile_while(while_stmt: &WhileStmt, asm: &mut Asm, ctx: &mut Context) {
    let mut label_count = asm.get_and_inc();
    let while_label = format!("L{}", label_count);
    label_count = asm.get_and_inc();
    let final_label = format!("L{}", label_count);
    asm.text.push(format!("{}:", while_label));
    match &while_stmt.condition {
        Expr::Bool(val) => {
            if *val {
                let mut block_ctx = ctx.clone();
                block_ctx.break_label = final_label.clone();
                block_ctx.continue_label = while_label.clone();
                compile_block(&while_stmt.block, asm, &mut block_ctx);
                asm.text.push(format!("\tjmp {}", while_label));
            } else {asm.text.push(format!("\tjmp {}", final_label));}
        },
        Expr::Bin(x) => {
            compare_exprs(&x.lhs, &x.rhs, asm, ctx);
            let cond_jmp: String = match x.op {
                TOK_GT => "jle",
                TOK_LT => "jge",
                TOK_GE => "jl",
                TOK_LE => "jg",
                TOK_EQ => "jne",
                TOK_NE => "je",
                _ => unreachable!(),
            }.to_string();
            asm.text.push(format!("\t{} {}", cond_jmp, final_label));
            let mut block_ctx = ctx.clone();
            block_ctx.break_label = final_label.clone();
            block_ctx.continue_label = while_label.clone();
            compile_block(&while_stmt.block, asm, &mut block_ctx);
            asm.text.push(format!("\tjmp {}", while_label));
        }
        _ => unreachable!(),
    };
    asm.text.push(format!("{}:", final_label));
}

fn compile_cf(cf_stmt: &CFStmt, asm: &mut Asm, ctx: &mut Context) {
    match cf_stmt {
        CFStmt::Return(_) => todo!("TODO: cf_stmt::Return(expr)"),
        CFStmt::Break => asm.text.push(format!("\tjmp {}", ctx.break_label)),
        CFStmt::Continue => asm.text.push(format!("\tjmp {}", ctx.continue_label)),
    };
}

fn compile(stmt: &Stmt, asm: &mut Asm, ctx: &mut Context) {
    match stmt {
        Stmt::VarAssign(x) => compile_var_assign(x, asm, ctx),
        Stmt::ExprStmt(x) => compile_expr(x, asm, ctx),
        Stmt::If(x) => compile_if(x, asm, ctx),
        Stmt::VarMut(x) => compile_var_mut(x, asm, ctx),
        Stmt::While(x) => compile_while(x, asm, ctx),
        Stmt::CF(x) => compile_cf(x, asm, ctx),
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
