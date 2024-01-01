use std::{process::exit, collections::HashSet};
use crate::fe::expr::{Expr, CallExpr};
use crate::compiler_felf64::compiler::{Asm, name_to_label};

// TODO: Create a utils a module containing these methods
fn error(message: String) {
    println!("error: {}", message);
    exit(1);
}
fn assert(eval: bool, message: String) {
    if !eval {error(message);}
}

fn compile_exit(call: &CallExpr, asm: &mut Asm) {
    assert(call.args.len() == 1, String::from("expected only one parameter"));
    if let Expr::Int(num) = call.args[0] {
	asm.text.push(String::from("\tmov rax, 60"));
	asm.text.push(format!("\tmov rdi, {}", num));
	asm.text.push(String::from("\tsyscall"));
    } else {error(String::from("expected NumExpr"));}
}

pub fn is_native_api(name: &String) -> bool {HashSet::from(["exit"]).contains(name.as_str())}

pub fn compile_api(call: &CallExpr, asm: &mut Asm) {
    match call.name.as_str() {
	"exit" => compile_exit(call, asm),
	_ => {},
    };
}
