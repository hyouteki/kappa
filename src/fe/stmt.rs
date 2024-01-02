use std::{fmt, collections::HashMap};
use crate::fe::expr::{Expr, parse_expr};
use crate::fe::lexer::{self, Lexer, token_kind_to_str};

pub struct Block {
    pub stmts: Vec<Stmt>,
    vars: HashMap<String, Expr>,
}

#[derive(Copy, Clone, PartialEq)]
pub enum Type {
    Str,
    Int,
    Bool,
}

pub struct Arg {
    pub name: String,
    pub arg_type: Type, 
}

pub struct FunDefStmt {
    pub name: String,
    pub args: Vec<Arg>,
    pub return_type: Type,
    pub block: Block,
}

pub struct VarAssignStmt {
    pub name: String,
    pub var_type: Type,
    pub expr: Expr,
    pub mutable: bool,  
}

pub struct IfStmt {
    pub condition: Expr,
    pub then_block: Block,
    pub else_block: Block,
}

pub struct WhileStmt {
    pub condition: Expr,
    pub block: Block,
}

pub enum CFStmt {
    Return(Expr),
    Break,
    Continue,
}

pub enum Stmt {
    BlockStmt(Box<Block>),
    FunDef(FunDefStmt),
    VarAssign(VarAssignStmt),
    If(IfStmt),
    While(WhileStmt),
    CF(CFStmt),
    ExprStmt(Expr),
}

impl Block {
    fn new(stmts: Vec<Stmt>) -> Self {
        Block{stmts: stmts, vars: HashMap::new()}
    }
    fn new_empty() -> Self {
        Block{stmts: Vec::new(), vars: HashMap::new()}
    }
    pub fn is_empty(&self) -> bool {
        self.stmts.is_empty()
    } 
}

impl VarAssignStmt {
    fn new(name: String, var_type: Type, expr: Expr, 
        mutable: bool) -> Self {
        VarAssignStmt{name: name, var_type: var_type, 
            expr: expr, mutable: mutable}
    }
}

impl FunDefStmt {
    fn new(name: String, args: Vec<Arg>, 
        return_type: Type, block: Block) -> Self {
        FunDefStmt{name: name, args: args, 
            return_type: return_type, block: block}
    }
}

impl IfStmt {
    fn new(condition: Expr, then_block: Block, else_block: Block) -> Self {
        IfStmt{condition: condition, then_block: then_block, else_block: else_block}
    }
}

impl WhileStmt {
    fn new(condition: Expr, block: Block) -> Self {
        WhileStmt{condition: condition, block: block}
    }
}

impl fmt::Display for VarAssignStmt {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let _ = write!(f, "VarAssignStmt ");
        if self.mutable {let _ = write!(f, "Mutable ");}
        write!(f, "Name({}): Type({}) = {}", self.name, 
            self.var_type, self.expr)
    }
}

impl fmt::Display for FunDefStmt {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let _ = write!(f, "FunDefStmt Name({}) Args(", self.name);
        for arg in self.args.iter() {
            let _ = write!(f, "{}, ", arg);
        }
        write!(f, "): ReturnType({}) {}", 
            self.return_type, self.block)
    }
}

impl fmt::Display for Type {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match &self {
            Type::Str => write!(f, "Str"),
            Type::Int => write!(f, "Int"),
            Type::Bool => write!(f, "Bool"),
        }
    }
}

impl fmt::Display for Arg {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}({})", self.arg_type, self.name)        
    }
}

impl fmt::Display for Block {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let _ = write!(f, "{{\n");
        for stmt in self.stmts.iter() {
            let _ = write!(f, "\t{}\n", stmt);
        }
        write!(f, "}}")
    }
}

impl fmt::Display for CFStmt {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            CFStmt::Return(x) => write!(f, "Return({})", x),
            CFStmt::Break => write!(f, "Break()"),
            CFStmt::Continue => write!(f, "Continue()"), 
        }
    }
}

impl fmt::Display for IfStmt {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let _ = write!(f, "IfStmt Condition({}) {}", 
            self.condition, self.then_block);
        match self.else_block.is_empty() {
            false => write!(f, " else {}", self.else_block),
            true => write!(f, ""),
        }
    }
}

impl fmt::Display for WhileStmt {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "WhileStmt Condition({}) {}", 
            self.condition, self.block)
    }
}

impl fmt::Display for Stmt {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match &self {
            Stmt::FunDef(x) => write!(f, "{}", x),
            Stmt::VarAssign(x) => write!(f, "{}", x),
            Stmt::BlockStmt(x) => write!(f, "{}", x),
            Stmt::ExprStmt(x) => write!(f, "{}", x),
            Stmt::If(x) => write!(f, "{}", x),
            Stmt::CF(x) => write!(f, "{}", x),
            _ => todo!("Not yet implemented"),
        }        
    }
}

fn token_kind_to_type(kind: i32) -> Option<Type> {
    match kind {
        lexer::TOK_TYPE_INT => Some(Type::Int),
        lexer::TOK_TYPE_STR => Some(Type::Str),
        lexer::TOK_TYPE_BOOL => Some(Type::Bool),
        _ => None
    }
}

fn parse_type(lexer: &mut Lexer) -> Type {
    let x: Option<Type> = token_kind_to_type(lexer.front().kind);
    if x.is_none() {lexer.error(String::from("invalid type"), None);}
    lexer.eat(); // eat type
    x.unwrap()
}

fn parse_block(lexer: &mut Lexer) -> Block {
    lexer.assert_token_kind('{' as i32);
    lexer.eat(); // eat '{'
    let mut stmts: Vec<Stmt> = Vec::new();
    while !lexer.empty() && !lexer.is_token_kind('}' as i32) {
        match parse_stmt(lexer) {
            Some(x) => stmts.push(x),
            None => {},
        }
        if !lexer.empty() && 
            lexer.is_token_kind('}' as i32) {break;}     
    }    
    lexer.eat(); // eat '}'
    Block::new(stmts)
}

pub fn parse_stmt(lexer: &mut Lexer) -> Option<Stmt> {
    let tok_colon: i32 = ';' as i32; 
    match lexer.front().kind {
        lexer::TOK_FN => parse_fun_def(lexer),
        lexer::TOK_VAR | lexer::TOK_VAL => parse_var_assign(lexer),
        lexer::TOK_IF => parse_if(lexer),
        lexer::TOK_ELSE => todo!("To be implemented"),
        lexer::TOK_WHILE => todo!("To be implemented"),
        lexer::TOK_RETURN => parse_return(lexer),
        x if x == tok_colon => {
            lexer.eat(); // eat ';'
            None
        }
        _ => {
            match parse_expr(lexer) {
                Some(x) => {return Some(Stmt::ExprStmt(x));},
                None => {},
            };
            lexer.error(String::from("invalid stmt"), None);
            None
        }
    }
}

fn parse_return(lexer: &mut Lexer) -> Option<Stmt> {
    lexer.assert_token_kind(lexer::TOK_RETURN);
    lexer.eat(); // eat return
    let expr: Expr = match parse_expr(lexer) {
        Some(x) => x,
        None => {lexer.error(String::from("invalid expr"), 
            None); unreachable!()},
    };
    Some(Stmt::CF(CFStmt::Return(expr)))
}

fn parse_if(lexer: &mut Lexer) -> Option<Stmt> {
    lexer.assert_token_kind(lexer::TOK_IF);
    lexer.eat(); // eat if
    let condition: Expr = match parse_expr(lexer) {
        Some(x) => x,
        None => {lexer.error(String::from("invalid expr"), 
            None); unreachable!()}
    };
    let then_block: Block = parse_block(lexer);
    if lexer.empty() || !lexer.is_token_kind(lexer::TOK_ELSE) {
        return Some(Stmt::If(IfStmt::new(condition, then_block, Block::new_empty())));
    }
    lexer.eat(); // eat else
    let else_block: Block = parse_block(lexer);
    Some(Stmt::If(IfStmt::new(condition, then_block, else_block))) 
}

fn parse_while(lexer: &mut Lexer) -> Option<Stmt> {
    lexer.assert_token_kind(lexer::TOK_WHILE);
    lexer.eat(); // eat while
    let condition: Expr = match parse_expr(lexer) {
        Some(x) => x,
        None => {lexer.error(String::from("invalid expr"), 
            None); unreachable!()}
    };
    let block: Block = parse_block(lexer);
    Some(Stmt::While(WhileStmt::new(condition, block)))
}

fn parse_var_assign(lexer: &mut Lexer) -> Option<Stmt> {
    let mutable: bool = match lexer.front().kind {
        lexer::TOK_VAR => true,
        lexer::TOK_VAL => false,
        x => {lexer.error(format!(
            "invalid token expected one of {}, {} found {}", 
            token_kind_to_str(lexer::TOK_VAR), 
            token_kind_to_str(lexer::TOK_VAL),
            token_kind_to_str(x)), None); unreachable!()}
    };
    lexer.eat(); // eat 'var' or 'val'
    lexer.assert_token_kind(lexer::TOK_IDEN);
    let name: String = lexer.front().get_str_val()
        .unwrap().to_string();
    lexer.eat(); // eat name
    lexer.assert_token_kind(':' as i32);
    lexer.eat();
    let var_type: Type = parse_type(lexer);
    lexer.assert_token_kind('=' as i32);
    lexer.eat(); // eat '='
    match parse_expr(lexer) {
        Some(x) => Some(Stmt::VarAssign(
            VarAssignStmt::new(name, var_type, x, mutable))),
        None => {lexer.error("invalid expr".to_string(), None); None},
    }
}

fn parse_fun_def(lexer: &mut Lexer) -> Option<Stmt> {
    lexer.assert_token_kind(lexer::TOK_FN);
    lexer.eat(); // eat fn
    lexer.assert_token_kind(lexer::TOK_IDEN);
    let name: String = lexer.front()
        .get_str_val().unwrap().to_string();
    lexer.eat(); // eat name
    lexer.assert_token_kind('(' as i32);
    lexer.eat(); // eat '('
    let mut args: Vec<Arg> = Vec::new();
    while !lexer.empty() && !lexer.is_token_kind(')' as i32) {
        lexer.assert_token_kind(lexer::TOK_IDEN);
        let arg_name: String = lexer.front()
            .get_str_val().unwrap().to_string();
        lexer.eat(); // eat arg_name
        lexer.assert_token_kind(':' as i32);
        lexer.eat(); // eat ':'
        lexer.assert_token();
        let arg_type: Type = parse_type(lexer);
        args.push(Arg{name: arg_name, arg_type: arg_type});
        if !lexer.empty() && lexer.is_token_kind(')' as i32) {break;}
        lexer.assert_token_kind(',' as i32);
        lexer.eat(); // eat ','
    }    
    lexer.eat(); // eat ')'
    lexer.assert_token_kind(':' as i32);
    lexer.eat(); // eat ':'
    let return_type: Type = parse_type(lexer);
    let block: Block = parse_block(lexer);
    Some(Stmt::FunDef(FunDefStmt::new(name, args, return_type, block)))
}
