use std::collections::HashMap;
use crate::fe::expr::{Expr, CallExpr, BinExpr};
use crate::fe::stmt::{VarAssignStmt, IfStmt, Stmt, Block, Arg, Type};
use crate::fe::lexer::{TOK_COMP_LOW, TOK_COMP_HIGH};
use crate::mw::native_api::get_native_apis;
use crate::utils::{error, assert};

#[derive(Clone)]
struct Var {
    expr_type: Type,
    mutable: bool,
}

#[derive(Clone)]
pub struct Fun {
    args: Vec<Arg>,
    return_type: Type,
}

#[derive(Clone)]
pub struct Context {
    vars: HashMap<String, Var>,
    funs: HashMap<String, Fun>,
}

impl Var {
    fn new(expr_type: Type, mutable: bool) -> Self {
        Var{expr_type: expr_type, mutable: mutable}
    }
}

impl Fun {
    pub fn new(args: Vec<Arg>, return_type: Type) -> Self {
        Fun{args: args, return_type: return_type}
    }
} 

impl Context {
    pub fn new() -> Self {
        Context{vars: HashMap::new(), funs: get_native_apis()}
    }
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
            let lhs_type: Type = get_expr_type(&x.lhs, ctx);
            let rhs_type: Type = get_expr_type(&x.rhs, ctx);
            assert(lhs_type == rhs_type,
                   format!("mismatch expr types {} and {}", lhs_type, rhs_type));
            match x.op {
                TOK_COMP_LOW..=TOK_COMP_HIGH => Type::Bool,
                _ => Type::Int,
            }
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
    };
}

fn validate_expr(expr: &Expr, ctx: &Context) {
    match expr {
        Expr::Call(x) => validate_fun_call(x, ctx),
        _ => {}
    }
}

fn block_validator(block: &Block, ctx: &mut Context) {
    for stmt in block.stmts.iter() {validate_stmt(stmt, ctx);}
}

fn validate_if_stmt(if_stmt: &IfStmt, ctx: &Context) {
    validate_type_with_expr(&Type::Bool, &if_stmt.condition, ctx);
    if !if_stmt.then_block.is_empty() {
        let mut then_block_ctx: Context = ctx.clone();
        block_validator(&if_stmt.then_block, &mut then_block_ctx);
    }
    if !if_stmt.else_block.is_empty() {
        let mut else_block_ctx: Context = ctx.clone();
        block_validator(&if_stmt.else_block, &mut else_block_ctx);
    }
}

fn validate_stmt(stmt: &Stmt, ctx: &mut Context) {
    match stmt {
	    Stmt::VarAssign(x) => validate_var_assign_stmt(x, ctx),
        Stmt::ExprStmt(x) => validate_expr(x, ctx),
        Stmt::If(x) => validate_if_stmt(x, ctx),
        _ => {},
    }
}

pub fn validator(stmts: &Vec<Stmt>) {
    let mut ctx: Context = Context::new();
    for stmt in stmts.iter() {validate_stmt(stmt, &mut ctx);}
}
