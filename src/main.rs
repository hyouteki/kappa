use std::{io::{prelude::*, BufReader}};

pub mod token;
pub mod lexer;
use lexer::{Lexer};

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
    println!("{:?}", content);
    // let lexer: Lexer = Lexer::new(content, filepath.to_string());
    // lexer.print();
}
