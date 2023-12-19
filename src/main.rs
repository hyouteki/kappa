use std::{io::{prelude::*, BufReader}};

pub mod fe;
pub mod transpiler_cpp;

use fe::{lexer::Lexer, parser::parse_lexer, stmt::Stmt};
use transpiler_cpp::transpiler;

fn print_usage() {
    println!("usage: kappa <subcommand> [<args>]");
    println!("subcommands: compile");
    println!("-\tcompile\t<filepath>");
    std::process::exit(1);
}

fn get_content(filepath: &str) -> Vec<String> {
    BufReader::new(std::fs::File::open(filepath)
        .expect("could not open file")).lines()
        .map(|line| line.expect("could not parse line"))
        .map(|line| line.replace("\t", "    ")) 
        .collect()
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 3 { print_usage(); }
    let filepath: &str  = &args[2];
    let content: Vec<String> = get_content(&filepath);
    let mut lexer: Lexer = Lexer::new(content, filepath.to_string());
    lexer.print();
    let stmts: Vec<Stmt> = parse_lexer(&mut lexer);
    println!("");
    for stmt in stmts.iter() {
        println!("{}", stmt);
    }
    println!("");
    transpiler(String::from("eg/main.cpp"), stmts);
}
