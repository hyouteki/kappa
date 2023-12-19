use std::{fs::File, io::Write};
use crate::stmt::{Stmt, FunDefStmt, VarAssignStmt, Type, Block};
use crate::expr::{Expr, CallExpr, BinExpr};

fn type_to_string(x: &Type) -> String {
    match x {
        Type::Str => String::from("std::string"),
        Type::Int => String::from("int"),
        Type::Bool => String::from("bool"), 
    }
}

fn bin_expr_to_string(expr: &BinExpr) -> String {
    let mut x = String::from("(");
    x.push_str(&expr_to_string(&expr.lhs));
    x.push(char::from_u32(expr.op.try_into().unwrap()).unwrap());
    x.push_str(&expr_to_string(&expr.rhs));
    x.push(')');
    x
}

fn call_expr_to_string(expr: &CallExpr) -> String {
    todo!("not yet implemented")
}

fn expr_to_string(expr: &Expr) -> String {
    match expr {
        Expr::Str(x) => x.to_string(),
        Expr::Int(x) => x.to_string(),
        Expr::Bool(x) => x.to_string(),
        Expr::Var(x) => x.to_string(),
        Expr::Bin(x) => bin_expr_to_string(x),
        Expr::Call(x) => call_expr_to_string(x),
        Expr::Null => String::from("null"),
    }
}

fn transpile_block(block: &Block, lines: &mut Vec<String>) {
    lines.push(String::from("{"));
    for stmt in block.stmts.iter() {
        transpile(&stmt, lines);
    }    
    lines.push(String::from("}"));
}

fn transpile_var_assign_stmt(var_assign: &VarAssignStmt, lines: &mut Vec<String>) {
    let mut line: String = String::from("");
    if var_assign.mutable {line.push_str("const ");}
    line.push_str(&type_to_string(&var_assign.var_type));
    line.push(' ');
    line.push_str(&var_assign.name);
    line.push_str(&String::from(" = "));
    line.push_str(&expr_to_string(&var_assign.expr));
    line.push(';');
    lines.push(line);
}

fn transpile_fun_def_stmt(fun_def: &FunDefStmt, lines: &mut Vec<String>) {
    let mut line: String = String::from(type_to_string(&fun_def.return_type));
    line.push(' ');
    line.push_str(&fun_def.name);
    line.push('(');
    for (i, arg) in fun_def.args.iter().enumerate() {
        line.push_str(&type_to_string(&arg.arg_type));
        line.push(' ');
        line.push_str(&arg.name);
        if i == fun_def.args.len()-1 {break;}
        line.push(',');
        line.push(' ');
    }
    line.push_str(&String::from(")"));
    lines.push(line);     
    transpile_block(&fun_def.block, lines);
}

fn transpile(stmt: &Stmt, lines: &mut Vec<String>) {
    match stmt {
        Stmt::FunDef(x) => transpile_fun_def_stmt(x, lines),
        Stmt::VarAssign(x) => transpile_var_assign_stmt(x, lines),
        _ => {}
    }
}

fn include_headers(lines: &mut Vec<String>) {
    lines.push(String::from("#include <string>"));
}

pub fn transpiler(filename: String, stmts: Vec<Stmt>) {   
    let mut lines: Vec<String> = Vec::new();
    include_headers(&mut lines);
    for stmt in stmts.iter() {
        transpile(stmt, &mut lines);
    }
    let mut file = File::create(filename).expect("could not create a file");
    for line in lines.iter() {
        file.write_all(line.as_bytes()).expect("could not write line");
        file.write_all(b"\n").expect("could not write new line");
    }
}
