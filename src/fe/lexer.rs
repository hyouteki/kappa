use crate::fe::token::{Loc, Token, error};

pub const TOK_FN: i32 = -1;
pub const TOK_IF: i32 = -2;
pub const TOK_ELSE: i32 = -3;
pub const TOK_WHILE: i32 = -4;
pub const TOK_VAR: i32 = -5;
pub const TOK_VAL: i32 = -6;
pub const TOK_RETURN: i32 = -7;
pub const TOK_BREAK: i32 = -8;
pub const TOK_CONTINUE: i32 = -9;
pub const TOK_TYPE_INT: i32 = -10;
pub const TOK_TYPE_STR: i32 = -11;
pub const TOK_TYPE_BOOL: i32 = -12;
pub const TOK_IDEN: i32 = -13;
pub const TOK_INT: i32 = -14; 
pub const TOK_STR_LIT: i32 = -15;
pub const TOK_BOOL: i32 = -16;
pub const TOK_COMP_HIGH: i32 = -17;
pub const TOK_GT: i32 = -17;
pub const TOK_LT: i32 = -18;
pub const TOK_GE: i32 = -19;
pub const TOK_LE: i32 = -20;
pub const TOK_EQ: i32 = -21;
pub const TOK_NE: i32 = -22;
pub const TOK_COMP_LOW: i32 = -22;

pub fn string_to_token_kind(s: &String) -> i32 {
    match s.as_str() {
        // keywords
        "fn" => TOK_FN, 
        "if" => TOK_IF, 
        "else" => TOK_ELSE, 
        "while" => TOK_WHILE, 
        "var" => TOK_VAR, 
        "val" => TOK_VAL,
        // control flow
        "return" => TOK_RETURN, 
        "break" => TOK_BREAK, 
        "continue" => TOK_CONTINUE, 
        // datatypes
        "int" => TOK_TYPE_INT, 
        "str" => TOK_TYPE_STR, 
        "bool" => TOK_TYPE_BOOL,
        // vals
        "true" => TOK_BOOL,
        "false" => TOK_BOOL, 
        _ => TOK_IDEN, // iden
    }
}

pub fn token_kind_to_str(kind: i32) -> String {
    match kind {
        TOK_FN => String::from("fn"),
        TOK_IF => String::from("if"),
        TOK_ELSE => String::from("else"),
        TOK_WHILE => String::from("while"),
        TOK_VAR => String::from("var"),
        TOK_VAL => String::from("val"),
        TOK_RETURN => String::from("return"),
        TOK_BREAK => String::from("break"),
        TOK_CONTINUE => String::from("continue"),
        TOK_TYPE_INT => String::from("int"),
        TOK_TYPE_STR => String::from("str"),
        TOK_TYPE_BOOL => String::from("bool"),
        TOK_IDEN => String::from("<iden>"),
        TOK_INT => String::from("<int>"), 
        TOK_STR_LIT => String::from("<str_lit>"),
        TOK_BOOL => String::from("<bool>"),
        TOK_GT => String::from(">="),
        TOK_LT => String::from("<="),
        TOK_GE => String::from(">"),
        TOK_LE => String::from("<"),
        TOK_EQ => String::from("=="),
        TOK_NE => String::from("!="),
        _ => char::from_u32(kind.try_into().unwrap()).unwrap().to_string(),
    }
}

pub struct Lexer {
    pub content: Vec<String>,
    pub filename: String,
    pub tokens: Vec<Token>,
}

impl Lexer {
    pub fn size(&self) -> usize {self.tokens.len()}
    pub fn empty(&self) -> bool {self.tokens.len() == 0}
    pub fn front(&self) -> Token {self.tokens[0].clone()}
    pub fn print(&self) {
        for token in self.tokens.iter() {
            println!("{}", token);
        }
    }
    pub fn assert_token(&self) {
        if self.empty() { 
            error("expected a token".to_string());
        }
    }
    pub fn eat(&mut self) {
        self.assert_token();
        self.tokens = self.tokens[1..].to_vec();
    }
    pub fn error(&self, message: String, loc: Option<Loc>) {
        print!("{}:", self.filename);
        match self.tokens.len() {
            0 => match loc {
                Some(x) => x.error(message),
                _ => error(message),
            },
            _ => self.front().loc.error(message),
        };
    }
    pub fn assert_token_kind(&self, kind: i32) {
        let token: Token = self.front();
        if token.kind != kind {
            token.loc.error(
                format!("expected kind {} found {}", 
                    token_kind_to_str(kind), 
                    token_kind_to_str(token.kind))
                .to_string());
        }
    }
    pub fn is_token_kind(&self, kind: i32) -> bool {
        self.front().kind == kind
    }
    fn gen_tokens(&mut self) {
        if self.content.len() == 0 {return;}
        for i in 0..self.content.len() {
            let mut col: usize = 0;
            while self.content[i].len() > 0 {
                // trimming leading white spaces
                {
                    let mut x: usize = 0;
                    for ch in self.content[i].chars() {
                        if ch != ' ' {break;}
                        x += 1;
                    } 
                    col += x;
                    self.content[i] = self.content[i][x as usize..]
                        .to_string();
                }

                if self.content[i].len() == 0 {break;}

                let mut j: usize = 0;
                let mut ch: char = self.content[i].chars()
                    .nth(j).unwrap();

                if ch.is_alphabetic() || ch == '_' {
                    let mut iden: String = String::from("");
                    while ch.is_alphanumeric() || ch == '_' {
                        iden.push(ch);
                        j += 1;
                        ch = self.content[i].chars().nth(j).unwrap();
                    } 
                    self.tokens.push(Token::new_str(
                        string_to_token_kind(&iden), iden, 
                        Loc::from_usize(i+1, col+1)));
                    col += j;
                    self.content[i] = self.content[i][j..].to_string();
                    continue;
                }
                
                if ch.is_numeric() {
                    let mut num: i32 = 0;
                    while ch.is_numeric() {
                        num = num*10 + ch as i32 - '0' as i32;
                        j += 1;
                        ch = self.content[i].chars().nth(j).unwrap();
                    }
                    self.tokens.push(Token::new_int(
                        TOK_INT, num, Loc::from_usize(
                            i+1, col+1)));
                    col += j;
                    self.content[i] = self.content[i][j..].to_string();
                    continue;
                }

                if ch == '"' {
                    let mut iden: String = String::from("");
                    let mut flag = true;
                    for k in 1..self.content[i].len() {
                        if self.content[i].chars().nth(k) == Some('"') {
                            flag = false;
                            break;
                        }
                        iden.push(self.content[i].chars().nth(k).unwrap());
                    }
                    if flag {
                        self.error("string literal not closed".to_string(), 
                            Some(Loc::from_usize(i+1, col+iden.len()+1 as usize))); 
                    }
                    j += iden.len() + 2; // +2 for ""
                    self.tokens.push(Token::new_str(TOK_STR_LIT, 
                        iden, Loc::from_usize(i+1, col+1)));
                    col += j;
                    self.content[i] = self.content[i][j..].to_string();
                    continue;
                }

                {
                    let syms = vec!['=', '<', '>', '!'];
                    if syms.contains(&ch) {
                        let mut sym = String::from("");
                        while syms.contains(&ch) {
                            sym.push(ch);
                            j += 1;
                            ch = self.content[i].chars().nth(j).unwrap();
                        }
                        if sym == "=" {
                            self.tokens.push(Token::new_str('=' as i32, sym,
                                Loc::from_usize(i+1, col+1)));
                            col += 1;
                            self.content[i] = self.content[i][1..].to_string();
                            continue;
                        }
                        let tok: i32 = match sym.as_str() {
                            ">"  => TOK_GT,
                            "<"  => TOK_LT,
                            ">=" => TOK_GE,
                            "<=" => TOK_LE,
                            "==" => TOK_EQ,
                            "!=" => TOK_NE,
                            _ => {
                                self.error(format!("invalid operator {}", sym), 
                                           Some(Loc::from_usize(i+1, col+1 as usize)));
                                unreachable!()
                            },
                        };
              
                        self.tokens.push(Token::new_str(
                            tok, sym, Loc::from_usize(i+1, col+1)));
                        col += j;
                        self.content[i] = self.content[i][j..].to_string();
                        continue;
                    }
                }
                
                {
                    self.tokens.push(Token::new_str(
                        ch as i32, ch.to_string(), 
                        Loc::from_usize(i+1, col+1)));
                    col += 1;
                    self.content[i] = self.content[i][1..].to_string();
                }
            }
        }
    }
    pub fn new(content: Vec<String>, filename: String) -> Self {
        let mut lexer: Lexer = Lexer{content: content, 
            filename: filename, tokens: Vec::new()};
        lexer.gen_tokens();
        lexer
    }
}
