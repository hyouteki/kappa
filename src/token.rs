#[derive(Clone)]
pub struct Loc {
    pub row: u32,
    pub col: u32,
}

impl std::fmt::Display for Loc {
    fn fmt(&self, f: &mut std::fmt::Formatter) 
        -> std::fmt::Result {
        write!(f, "{}:{}", self.row, self.col)
    }
}

impl Loc {
    pub fn new(row: u32, col: u32) -> Self {
        Loc{row: row, col: col}
    }
    pub fn from_usize(row: usize, col: usize) -> Self {
        Loc{row: row.try_into().unwrap(), 
            col: col.try_into().unwrap()}
    }
    pub fn error(&self, message: String) {
        println!("{}:{}: error: {}", 
            self.row, self.col, message);        
    }
}

#[derive(Clone)]
pub enum TokenVal {
    StrVal(String),
    IntVal(i32),
    BoolVal(bool),
    Null,
}

#[derive(Clone)]
pub struct Token {
    pub kind: i32,
    pub val: TokenVal,
    pub loc: Loc,
}

impl std::fmt::Display for Token {
    fn fmt(&self, f: &mut std::fmt::Formatter)
        -> std::fmt::Result {
        write!(f, "{}\t", self.loc)?;
        let _ = match &self.val {
            TokenVal::StrVal(x) => write!(f, "{}", x),
            TokenVal::IntVal(x) => write!(f, "{}", x),
            TokenVal::BoolVal(x) => write!(f, "{}", x),
            TokenVal::Null => write!(f, "null"),
        };
        write!(f, "\t{}", self.kind)
    }
}

impl Token {
    pub fn new_str(kind: i32, val: String, loc: Loc) -> Self {
        Token{kind: kind, val: TokenVal::StrVal(val), loc: loc}
    }

    pub fn new_int(kind: i32, val: i32, loc: Loc) -> Self {
        Token{kind: kind, val: TokenVal::IntVal(val), loc: loc}
    }

    pub fn get_str_val(&self) -> Option<&String> {
        match &self.val {
            TokenVal::StrVal(str_val) => Some(str_val),
            _ => None,
        }
    }

    pub fn get_int_val(&self) -> Option<&i32> {
        match &self.val {
            TokenVal::IntVal(int_val) => Some(int_val),
            _ => None,
        }
    }

    pub fn get_bool_val(&self) -> Option<&bool> {
        match &self.val {
            TokenVal::BoolVal(bool_val) => Some(bool_val),
            _ => None,
        }
    }
}

pub fn error(message: String) {
    println!("error: {}", message);
    std::process::exit(1);
}
