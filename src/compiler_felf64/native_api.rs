use std::{process::exit, collections::HashSet};
use crate::fe::expr::{Expr, CallExpr};
use crate::compiler_felf64::compiler::{Asm, name_to_label};
use crate::utils::{error, assert};

fn compile_exit(call: &CallExpr, asm: &mut Asm) {
    if let Expr::Int(num) = call.args[0] {
	    asm.text.push(String::from("\tmov rax, 60"));
	    asm.text.push(format!("\tmov rdi, {}", num));
	    asm.text.push(String::from("\tsyscall"));
    }
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

pub fn compile_api(call: &CallExpr, asm: &mut Asm) {
    match call.name.as_str() {
	    "exit" => compile_exit(call, asm),
        "print" => compile_print(call, asm),
	    _ => {},
    };
}
