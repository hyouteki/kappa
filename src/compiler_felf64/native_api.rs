use std::{process::exit, collections::HashSet};
use crate::fe::expr::{Expr, CallExpr};
use crate::compiler_felf64::compiler::{Asm, Context};
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

fn compile_print(call: &CallExpr, asm: &mut Asm) {
    if let Expr::Str(text) = &call.args[0] {
        let label_count = asm.get_and_inc();
        // TODO: handle excape characters
        asm.data.push(format!("L{}:", label_count));
        asm.data.push(format!("\tdw '{}'", text));
        asm.text.push(format!("\tmov rax, 1"));
	    asm.text.push(format!("\tmov rdi, 1"));
	    asm.text.push(format!("\tmov rsi, L{}", label_count));
	    asm.text.push(format!("\tmov rdx, {}", text.len()+1));
        asm.text.push(format!("\tsyscall"));
    }
}

pub fn is_native_api(name: &String) -> bool {
    HashSet::from(["exit", "print"]).contains(name.as_str())
}

pub fn compile_api(call: &CallExpr, asm: &mut Asm, ctx: &mut Context) {
    match call.name.as_str() {
	    "exit" => compile_exit(call, asm, ctx),
        "print" => compile_print(call, asm),
	    _ => {},
    };
}
