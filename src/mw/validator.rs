use std::collections::HashMap;
use crate::fe::expr::{Expr, CallExpr};
use crate::fe::stmt::{VarAssignStmt, Stmt, Arg, Type};
use crate::utils::{error, assert};

struct Var {
    expr_type: Type,
    mutable: bool,
}

struct Fun {
    args: Vec<Arg>,
    return_type: Type,
}

pub struct Context {
    vars: HashMap<String, Var>,
    funs: HashMap<String, Fun>,
}

impl Var {
    fn new(expr_type: Type, mutable: bool) -> Self {
        Var{expr_type: expr_type, mutable: mutable}
    }
}

impl Context {pub fn new() -> Self {
    Context{vars: HashMap::new(), funs: HashMap::new()}}
}

fn get_expr_type(expr: &Expr, ctx: &Context) -> Type {
    match expr {
	    Expr::Str(_) => Type::Str,
	    Expr::Int(_) => Type::Int,
	    Expr::Bool(_) => Type::Bool,
	    Expr::Var(name) => match ctx.vars.get(name) {
  	        Some(x) => x.expr_type,
  	        None => {
		        error(String::from("variable does not exist"));
		        unreachable!()
	        }
	    }
	    Expr::Bin(x) => {
	        todo!("bin expr validation is not implemented")
	    },
	    Expr::Call(call_expr) => match ctx.funs.get(&call_expr.name) {
  	        Some(x) => {
		        validate_fun_call(call_expr, ctx);
		        x.return_type
	        },
  	        None => {
		        error(format!("function {} does not exist", call_expr.name));
		        unreachable!()
	        }
	    },
	    Expr::Null => todo!("null valued expr validation is not implemented")
    }
}

fn validate_type_with_expr(expr_type: &Type, expr: &Expr, ctx: &Context) {
    let type_found: Type = get_expr_type(expr, ctx);
    assert(expr_type == &type_found,
           format!("expected type {} found {}", expr_type, type_found));
}

fn validate_var_assign_stmt(stmt: &VarAssignStmt, ctx: &mut Context) {
    validate_type_with_expr(&stmt.var_type, &stmt.expr, ctx);
    ctx.vars.insert(String::from(&stmt.name), Var::new(stmt.var_type, stmt.mutable));
}

fn validate_fun_call(call: &CallExpr, ctx: &Context) {
    match ctx.funs.get(&call.name) {
	    Some(fun) => {
            assert(fun.args.len() == call.args.len(),
                   String::from("arguments size mismatch"));
            for i in 0..fun.args.len() {
		        validate_type_with_expr(&fun.args[i].arg_type, &call.args[i], ctx);
	        }
	    },
	    None => error(format!("function {} does not exist", call.name)),
    }
}

fn validate_stmt(stmt: &Stmt, ctx: &mut Context) {
    match stmt {
	    Stmt::VarAssign(x) => validate_var_assign_stmt(x, ctx),
	    _ => {},
    }
}

pub fn validator(stmts: &Vec<Stmt>, ctx: &mut Context) {
    for stmt in stmts.iter() {validate_stmt(stmt, ctx);}
}
