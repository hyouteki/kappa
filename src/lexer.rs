use crate::token::{Loc, Token, error};

fn string_to_token_kind(s: &String) -> i32 {
    match s.as_str() {
        // keywords
        "fn" => -1, 
        "if" => -2, 
        "else" => -3, 
        "while" => -4, 
        "var" => -5, 
        "val" => -6,
        // control flow
        "return" => -7, 
        "break" => -8, 
        "continue" => -9, 
        // datatypes
        "int" => -10, 
        "str" => -11, 
        "bool" => -12, 
        _ => -13, // iden
    }
}

const TOK_INT: i32 = -14; 
const TOK_STR_LIT: i32 = -15;

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
    pub fn assert_token_front(&self) {
        if self.empty() { 
            error("expected a token".to_string());
        }
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
    pub fn assert_token_kind_front(&self, kind: i32) {
        let token: Token = self.front();
        if token.kind != kind {
            token.loc.error(
                "expected kind {kind} found {token.kind}"
                .to_string());
        }
    }
    pub fn is_token_kind_front(&self, kind: i32) -> bool {
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

                if ch.is_alphanumeric() || ch == '_' {
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
                    let mut iden: String = String::from("\"");
                    for k in 1..self.content[i].len() {
                        iden.push(self.content[i].chars().nth(k).unwrap());
                        if self.content[i].chars().nth(k) == Some('"') {break};
                    }
                    if iden.chars().last().unwrap() != '"' {
                        self.error("string literal not closed"
                            .to_string(), Some(Loc::from_usize(i+1, 
                                col+iden.len()+1 as usize))); 
                    }
                    j += iden.len();
                    self.tokens.push(Token::new_str(TOK_STR_LIT, 
                        iden, Loc::from_usize(i+1, col+1)));
                    col += j;
                    self.content[i] = self.content[i][j..].to_string();
                    continue;
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
