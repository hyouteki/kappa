use std::fmt::{Display, Formatter};
use crate::lexer::{self, Lexer};

pub struct BinExpr {
    left: Expr,
    op: i32,
    right: Expr,
}

pub struct CallExpr {
    name: String,
    args: Vec<Expr>,
}

pub enum Expr {
    Str(String),
    Int(i32),
    Bool(bool),
    Var(String),
    Bin(Box<BinExpr>),
    Call(Box<CallExpr>),
    Null,
}

impl Display for Expr {
    fn fmt(&self, f: &mut Formatter) -> std::fmt::Result {
        match &self {
            Expr::Str(x) => write!(f, "StrExpr(\"{}\")", x),
            Expr::Int(x) => write!(f, "IntExpr({})", x),
            Expr::Bool(x) => write!(f, "BoolExpr({})", x),
            Expr::Var(x) => write!(f, "VarExpr({})", x),
            Expr::Bin(x) => write!(f, "{}", x),
            Expr::Call(x) => write!(f, "{}", x),
            Expr::Null => write!(f, "NullExpr()"),
        }
    }
}

impl Display for BinExpr {
    fn fmt(&self, f: &mut Formatter) -> std::fmt::Result {
        write!(f, "BinExpr({}, Op({}), {})", self.left, 
            std::char::from_u32(self.op.try_into()
                .unwrap()).unwrap(), self.right)
    }
}

impl BinExpr {
    pub fn new(left: Expr, op: i32, right: Expr) -> Self {
        Self{left: left, op: op, right: right}
    }
}

impl Display for CallExpr {
    fn fmt(&self, f: &mut Formatter) -> std::fmt::Result {
        let _ = write!(f, "FunCall({}(", self.name);
        for expr in self.args.iter() {
            let _ = write!(f, "{expr}, ");
        }
        write!(f, "))")
    }
}

impl CallExpr {
    pub fn new(name: String, args: Vec<Expr>) -> Self {
        CallExpr{name: name, args: args}
    }
}

pub fn parse_num_expr(lexer: &mut Lexer) -> Option<Expr> {
    lexer.assert_token();
    let expr: Option<Expr> = match lexer.front()
        .get_int_val() {
            Some(x) => Some(Expr::Int(*x)),
            None => None,
        };
    lexer.eat();
    expr
}

pub fn parse_str_expr(lexer: &mut Lexer) -> Option<Expr> {
    lexer.assert_token();
    let expr: Option<Expr> = match lexer.front()
        .get_str_val() {
            Some(x) => Some(Expr::Str(x.to_string())),
            None => None,
        };
    lexer.eat();
    expr
}

pub fn parse_bool_expr(lexer: &mut Lexer) -> Option<Expr> {
    lexer.assert_token();
    let expr: Option<Expr> = match lexer.front()
        .get_bool_val() {
            Some(x) => Some(Expr::Bool(*x)),
            None => None,
        };
    lexer.eat();
    expr
}

pub fn parse_iden(lexer: &mut Lexer) -> Option<Expr> {
    lexer.assert_token();
    let name: String = lexer.front().get_str_val()
        .unwrap().to_string();
    lexer.eat(); // eat name
    if lexer.empty() || !lexer.is_token_kind('(' as i32) {
        return Some(Expr::Var(name));
    }
    lexer.eat(); // eat '('
    let mut args: Vec<Expr> = Vec::new();
    while !lexer.empty() && !lexer.is_token_kind(')' as i32) {
        match parse_expr(lexer) {
            Some(x) => args.push(x),
            None => lexer.error(
                String::from("expected correct expr"), None),
        };
        if !lexer.empty() && 
            lexer.is_token_kind(')' as i32) {break;}
        lexer.assert_token_kind(',' as i32);
        lexer.eat(); // eat ','
    }
    lexer.eat(); // eat ')'
    Some(Expr::Call(Box::new(CallExpr{name: name, args: args})))
}

pub fn parse_expr(lexer: &mut Lexer) -> Option<Expr> {
    lexer.assert_token();
    match lexer.front().kind {
        lexer::TOK_INT => parse_num_expr(lexer),
        lexer::TOK_STR_LIT => parse_str_expr(lexer),
        lexer::TOK_BOOL => parse_bool_expr(lexer),
        lexer::TOK_IDEN => parse_iden(lexer),
        _ => Some(Expr::Null)   
    }
}
