pub mod validator;
pub mod native_api;
pub mod compile_time_eval;

use crate::fe::stmt::Stmt;
use crate::mw::validator::validator;
use crate::mw::compile_time_eval::compile_time_eval;

pub fn middleware(stmts: &mut Vec<Stmt>) {
	validator(stmts);
	compile_time_eval(stmts);
}

