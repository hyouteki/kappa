use std::fmt::{Display, Formatter};

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
            Expr::Str(x) => write!(f, "StrExpr({})", x),
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
