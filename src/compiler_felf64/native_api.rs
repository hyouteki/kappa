use std::{process::exit, collections::HashSet};
use crate::fe::expr::{Expr, CallExpr};
use crate::compiler_felf64::compiler::{Asm, Var, Context};
use crate::utils::{error, assert};

fn compile_exit(call: &CallExpr, asm: &mut Asm, ctx: &Context) {
    asm.text.push("\tmov rax, 60".to_string());
    match &call.args[0] {
        Expr::Int(num) => asm.text.push(format!("\tmov edi, {}", num)),
        Expr::Var(name) => asm.text.push(format!(
            "\tmov edi, DWORD [rbp-{}]",
            ctx.vars.get(name).unwrap().bp_offset)),
        _ => unreachable!(),
    };
    asm.text.push("\tsyscall".to_string());
}

fn compile_print(call: &CallExpr, asm: &mut Asm, ctx: &Context) {
    match &call.args[0] {
        Expr::Str(text) => {
            let label_count = asm.get_and_inc();
            asm.data.extend(vec![
                format!("L{}:", label_count),
                format!("\tdb `{}`", text),
            ]);
            asm.text.extend(vec![
                "\tmov rax, 1".to_string(),
                "\tmov rdi, 1".to_string(),
                format!("\tmov rsi, L{}", label_count),
                format!("\tmov rdx, {}", text.len()+1),
                "\tsyscall".to_string(),
            ]);
        },
        Expr::Var(name) => {
            let var = ctx.vars.get(name).unwrap();
            asm.text.extend(vec![
                "\tmov rax, 1".to_string(),
                "\tmov rdi, 1".to_string(),
                format!("\tmov rsi, QWORD [rbp-{}]", var.bp_offset),
                format!("\tmov edx, DWORD [rbp-{}]", var.bp_offset-8),
                "\tsyscall".to_string(),
            ]);
        }
        _ => unreachable!(),
    };
}

pub fn is_native_api(name: &String) -> bool {
    HashSet::from(["exit", "print"]).contains(name.as_str())
}

pub fn compile_api(call: &CallExpr, asm: &mut Asm, ctx: &mut Context) {
    match call.name.as_str() {
	    "exit" => compile_exit(call, asm, ctx),
        "print" => compile_print(call, asm, ctx),
	    _ => {},
    };
}
