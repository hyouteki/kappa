use std::collections::HashMap;
use crate::fe::expr::{Expr, BinExpr, CallExpr};
use crate::fe::stmt::{Stmt, FunDefStmt, VarAssignStmt, IfStmt,
					  WhileStmt, Block, VarMutStmt};
use crate::fe::lexer::{TOK_LE, TOK_GE, TOK_LT, TOK_GT, TOK_EQ, TOK_NE};
use crate::utils::error;

#[derive(Clone)]
struct Var {
	expr: Expr,
}

impl Var {fn new(expr: Expr) -> Self {Var{expr: expr}}}

#[derive(Clone)]
struct Context {
	vars: HashMap<String, Var>,
}

impl Context {fn new() -> Self {Context{vars: HashMap::new()}}}

fn is_primary_expr(expr: &Expr) -> bool {
	match expr {
		Expr::Str(_) | Expr::Int(_) | Expr::Bool(_) => true,
		_ => false,
	}
}

fn eval_int_bin_expr(a: i32, b: i32, op: i32) -> Expr {
	if op < 0 {
		Expr::Bool(match op {
			TOK_LE => a <= b,
			TOK_GE => a >= b,
			TOK_GT => a > b,
			TOK_LT => a < b,
			TOK_EQ => a == b,
			TOK_NE => a != b,
			_ => {error("invalid binary operator".to_string()); unreachable!()},
		})
	} else {
		let ch = char::from_u32(op.try_into().unwrap()).unwrap();
		Expr::Int(match ch {
			'+' => a+b,
			'-' => a-b,
			'*' => a*b,
			'/' => a/b,
			_ => {error(format!("invalid binary operator '{}'", ch)); unreachable!()},
		})
	}
}

fn eval_str_bin_expr(a: String, b: String, op: i32) -> Expr {
	if op < 0 {
		Expr::Bool(match op {
			TOK_LE => a <= b,
			TOK_GE => a >= b,
			TOK_GT => a > b,
			TOK_LT => a < b,
			TOK_EQ => a == b,
			TOK_NE => a != b,
			_ => {error("invalid binary operator".to_string()); unreachable!()},
		})
	} else {
		let ch = char::from_u32(op.try_into().unwrap()).unwrap();
		Expr::Str(match ch {
			'+' => a+&b,
			_ => {error(format!("invalid binary operator '{}'", ch)); unreachable!()},
		})
	}
}

fn eval_bool_bin_expr(a: bool, b: bool, op: i32) -> Expr {
	if op < 0 {
		Expr::Bool(match op {
			TOK_LE => a <= b,
			TOK_GE => a >= b,
			TOK_GT => a > b,
			TOK_LT => a < b,
			TOK_EQ => a == b,
			TOK_NE => a != b,
			_ => {error("invalid binary operator".to_string()); unreachable!()},
		})
	} else {
		let ch = char::from_u32(op.try_into().unwrap()).unwrap();
		error(format!("invalid binary operator '{}'", ch));
		unreachable!()
	}
}


fn eval_bin_expr(expr: &BinExpr, ctx: &Context) -> Expr {
	let (lhs, rhs) = (eval_expr(&expr.lhs, ctx), eval_expr(&expr.rhs, ctx));
	if !is_primary_expr(&lhs) || !is_primary_expr(&rhs) {
		return Expr::Bin(Box::new(BinExpr::new(lhs, expr.op, rhs)));
	}
	match (lhs, rhs) {
		(Expr::Int(a), Expr::Int(b)) => eval_int_bin_expr(a, b, expr.op),
		(Expr::Str(a), Expr::Str(b)) => eval_str_bin_expr(a, b, expr.op),
		(Expr::Bool(a), Expr::Bool(b)) => eval_bool_bin_expr(a, b, expr.op),
		_ => unreachable!(),
	}
}

fn eval_call_expr(call_expr: &CallExpr, ctx: &Context) -> Expr {
	let mut args: Vec<Expr> = Vec::new();
	for expr in call_expr.args.iter() {
		args.push(eval_expr(expr, ctx));
	}
	Expr::Call(Box::new(CallExpr::new(call_expr.name.to_string(), args)))
}

fn eval_expr(expr: &Expr, ctx: &Context) -> Expr {
	match expr {
		Expr::Var(name) => eval_expr(&ctx.vars.get(name).unwrap().expr, ctx),
		Expr::Bin(bin_expr) => eval_bin_expr(&bin_expr, ctx),
		Expr::Call(call_expr) => eval_call_expr(&call_expr, ctx),
		Expr::Str(_) | Expr::Int(_) | Expr::Bool(_) | Expr::Null => expr.clone(),
	}
}

fn eval_fun_def_stmt(fun_def: &mut FunDefStmt, ctx: &mut Context) {
	eval_block(&mut fun_def.block, ctx);
}

fn eval_var_assign_stmt(var_assign: &mut VarAssignStmt, ctx: &mut Context) {
	var_assign.expr = eval_expr(&var_assign.expr, ctx);
	ctx.vars.insert(var_assign.name.to_string(), Var::new(var_assign.expr.clone()));
}

fn eval_if_stmt(if_stmt: &mut IfStmt, ctx: &mut Context) {
	if_stmt.condition = eval_expr(&if_stmt.condition, ctx);
	eval_block(&mut if_stmt.then_block, ctx);
	eval_block(&mut if_stmt.else_block, ctx);
}


fn eval_while_stmt(while_stmt: &mut WhileStmt, ctx: &mut Context) {
	while_stmt.condition = eval_expr(&while_stmt.condition, ctx);
	eval_block(&mut while_stmt.block, ctx);
}

fn eval_block(block: &mut Block, ctx: &mut Context) {
	let mut child_ctx = ctx.clone();
	for stmt in block.stmts.iter_mut() {
		eval_stmt(stmt, &mut child_ctx);
	}
}

fn eval_var_mut_stmt(var_mut: &mut VarMutStmt, ctx: &mut Context) {
	var_mut.expr = eval_expr(&var_mut.expr, ctx);
}

fn eval_expr_stmt(expr: &mut Expr, ctx: &mut Context) {
	*expr = eval_expr(expr, ctx);
}

fn eval_stmt(stmt: &mut Stmt, ctx: &mut Context) {
	match stmt {
		Stmt::FunDef(x) => eval_fun_def_stmt(x, ctx),
		Stmt::VarAssign(x) => eval_var_assign_stmt(x, ctx),
		Stmt::If(x) => eval_if_stmt(x, ctx),
		Stmt::While(x) => eval_while_stmt(x, ctx),
		Stmt::VarMut(x) => eval_var_mut_stmt(x, ctx),
		Stmt::ExprStmt(x) => eval_expr_stmt(x, ctx),
		_ => todo!("compile_time_eval::eval_stmt::Exhaustive"),
	}
}

pub fn compile_time_eval(stmts: &mut Vec<Stmt>) {
	let mut ctx: Context = Context::new();
	for stmt in stmts.iter_mut() {
		eval_stmt(stmt, &mut ctx);
	}
}
