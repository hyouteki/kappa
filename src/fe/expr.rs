use std::fmt;
use crate::fe::lexer::{self, Lexer};

#[derive(Clone)]
pub struct BinExpr {
    pub lhs: Expr,
    pub op: i32,
    pub rhs: Expr,
}

#[derive(Clone)]
pub struct CallExpr {
    pub name: String,
    pub args: Vec<Expr>,
}

#[derive(Clone)]
pub enum Expr {
    Str(String),
    Int(i32),
    Bool(bool),
    Var(String),
    Bin(Box<BinExpr>),
    Call(Box<CallExpr>),
    Null,
}

fn get_op_prec(op: i32) -> i32 {
    if op < 0 {
        match op {
            lexer::TOK_COMP_LOW..=lexer::TOK_COMP_HIGH => 20,
            _ => -1,
        }
    } else {
        let ch: char = char::from_u32(op.try_into().unwrap()).unwrap();
        match ch {
            '*' | '/' => 60,
            '+' | '-' => 40,
            _ => -1,
        }
    }
}

impl fmt::Display for Expr {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
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

impl fmt::Display for BinExpr {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "BinExpr({}, Op({}), {})", self.lhs, 
            lexer::token_kind_to_str(self.op), self.rhs)
    }
}

impl BinExpr {
    pub fn new(lhs: Expr, op: i32, rhs: Expr) -> Self {
        Self{lhs: lhs, op: op, rhs: rhs}
    }
}

impl fmt::Display for CallExpr {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
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
    let expr: Option<Expr> = match lexer.front().get_str_val().unwrap().as_str() {
        "true" => Some(Expr::Bool(true)),
        "false" => Some(Expr::Bool(false)),
        _ => None,
    };
    lexer.eat();
    expr
}

pub fn parse_paren_expr(lexer: &mut Lexer) -> Option<Expr> {
    lexer.assert_token_kind('(' as i32);
    lexer.eat(); // eat '('
    let expr: Option<Expr> = parse_expr(lexer);
    lexer.assert_token_kind(')' as i32);
    lexer.eat(); // eat ')'
    expr
}

// reference: https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl02.html
pub fn parse_bin_rhs(lexer: &mut Lexer, _cur_prec: i32, lhs: Expr) -> Option<Expr> {
    loop {
        if lexer.empty() {return Some(lhs);}
        let bin_op: i32 = lexer.front().kind;
        let tok_prec: i32 =  get_op_prec(bin_op);
        if tok_prec < 0 {return Some(lhs);}
        lexer.eat(); // eat bin_op
        let mut rhs: Expr = match parse_primary_expr(lexer) {
            Some(x) => x,
            None => {lexer.error("invalid expr".to_string(), 
                None); unreachable!()}
        };
        if lexer.empty() {
            return Some(Expr::Bin(Box::new(BinExpr::new(lhs, bin_op, rhs))));
        }
        let next_op: i32 = lexer.front().kind;
        let next_prec: i32 = get_op_prec(next_op);
        if tok_prec < next_prec {
            rhs = match parse_bin_rhs(lexer, tok_prec+1, rhs) {
                Some(x) => x,
                None => {lexer.error("invalid expr".to_string(), 
                    None); unreachable!()}
            };
        }
        let new_lhs = Expr::Bin(Box::new(BinExpr::new(lhs, bin_op, rhs)));
        return parse_bin_rhs(lexer, tok_prec+1, new_lhs);    
    }
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

fn parse_primary_expr(lexer: &mut Lexer) -> Option<Expr> {
    lexer.assert_token();
    match lexer.front().kind {
        lexer::TOK_INT => parse_num_expr(lexer),
        lexer::TOK_STR_LIT => parse_str_expr(lexer),
        lexer::TOK_BOOL => parse_bool_expr(lexer),
        lexer::TOK_IDEN => parse_iden(lexer),
        x if x == '(' as i32 => parse_paren_expr(lexer),
        _ => Some(Expr::Null)   
    }
}

pub fn parse_expr(lexer: &mut Lexer) -> Option<Expr> {
    match parse_primary_expr(lexer) {
        Some(x) => parse_bin_rhs(lexer, 0, x),
        None => None,
    }
}
