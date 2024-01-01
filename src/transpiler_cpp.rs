use std::{fs::File, io::Write};
use crate::fe::stmt::{Stmt, CFStmt, FunDefStmt, IfStmt, 
    WhileStmt, VarAssignStmt, Type, Block};
use crate::fe::expr::{Expr, CallExpr, BinExpr};

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
    let mut x = String::from(&expr.name);
    x.push('(');
    for (i, arg) in expr.args.iter().enumerate() {
        x.push_str(&expr_to_string(&arg));
        if i == expr.args.len()-1 {break};
        x.push(',');
        x.push(' ');
    }
    x.push(')');
    x
}

fn expr_to_string(expr: &Expr) -> String {
    match expr {
        Expr::Str(x) => {
            let mut line: String = String::from("\"");
            line.push_str(&x);
            line.push('"');
            line
        },
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

fn transpile_cf_stmt(cf: &CFStmt, lines: &mut Vec<String>) {
    match cf {
        CFStmt::Return(x) => {
            let mut line: String = String::from("return ");
            line.push_str(&expr_to_string(x));
            line.push(';');
            lines.push(line);
        },
        CFStmt::Break => lines.push(String::from("break;")),
        CFStmt::Continue => lines.push(String::from("continue;")),
    };
}

fn transpile_if_stmt(if_stmt: &IfStmt, lines: &mut Vec<String>) {
    lines.push(format!("if ({})", expr_to_string(&if_stmt.condition)));
    transpile_block(&if_stmt.then_block, lines);
    if !if_stmt.else_block.is_empty() {
        lines.push(String::from("else"));
        transpile_block(&if_stmt.else_block, lines);
    } 
}

fn transpile_while_stmt(while_stmt: &WhileStmt, lines: &mut Vec<String>) {
    lines.push(format!("while ({})", expr_to_string(&while_stmt.condition)));
    transpile_block(&while_stmt.block, lines);    
}

fn transpile_to_native_api(call: &CallExpr, lines: &mut Vec<String>) {
    let mut x: String = String::from("");
    match call.name.as_str() {
        "print" => {
            x.push_str(&String::from("std::cout"));
            for arg in call.args.iter() {
                x.push_str(&String::from(" << ")); 
                x.push_str(&expr_to_string(&arg));
            }
            x.push(';');
        },
        _ => x.push_str(&call_expr_to_string(call)),
    };
    lines.push(x);
}

fn transpile_expr_stmt(expr: &Expr, lines: &mut Vec<String>) {
    match expr {
        Expr::Call(x) => transpile_to_native_api(x, lines),
        _ => {},        
    };
}

fn transpile(stmt: &Stmt, lines: &mut Vec<String>) {
    match stmt {
        Stmt::FunDef(x) => transpile_fun_def_stmt(x, lines),
        Stmt::VarAssign(x) => transpile_var_assign_stmt(x, lines),
        Stmt::ExprStmt(x) => transpile_expr_stmt(x, lines),
        Stmt::If(x) => transpile_if_stmt(x, lines),
        Stmt::While(x) => transpile_while_stmt(x, lines),
        Stmt::CF(x) => transpile_cf_stmt(x, lines),
        _ => {}
    };
}

fn include_headers(lines: &mut Vec<String>) {
    lines.push(String::from("#include <cstdio>"));
    lines.push(String::from("#include <cstdlib>"));
    lines.push(String::from("#include <string>"));
    lines.push(String::from("#include <iostream>"));
}

pub fn transpiler(filename: String, stmts: &Vec<Stmt>) {   
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
