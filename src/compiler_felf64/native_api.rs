use std::{process::exit, collections::HashSet};
use crate::fe::expr::{Expr, CallExpr};
use crate::compiler_felf64::compiler::{Asm, Var, Context, access_expr_val};
use crate::utils::{error, assert, strlen};

fn rbx_or_ebx(value: &String) -> String {
    if value.starts_with("DWORD") {"ebx".to_string()}
    else {"rbx".to_string()}
}

fn compile_exit(call: &CallExpr, asm: &mut Asm, ctx: &mut Context) {
    let result: String = access_expr_val(&call.args[0], asm, ctx);
    asm.text.extend(vec![
        format!("\tmov {}, {}", rbx_or_ebx(&result), result),
        "\tmov rax, 60".to_string(),
        "\tmov rdi, rbx".to_string(),
        "\tsyscall".to_string(),
    ]);
}

fn compile_print(call: &CallExpr, asm: &mut Asm, ctx: &mut Context) {
    match &call.args[0] {
        Expr::Str(text) => {
            let label_count = asm.get_and_inc();
            asm.data.extend(vec![
                format!("L{}:", label_count),
                format!("\tdb `{}`, 0", text),
            ]);
            asm.text.extend(vec![
                "\tmov rax, 1".to_string(),
                "\tmov rdi, 1".to_string(),
                format!("\tmov rsi, L{}", label_count),
                format!("\tmov rdx, {}", strlen(text)+1),
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
