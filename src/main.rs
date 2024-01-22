use std::{io::{prelude::*, BufReader}};
use clap::{Arg, Command, ArgAction, ArgGroup};

pub mod fe;
pub mod transpiler_cpp;
pub mod compiler_felf64;
pub mod mw;
pub mod utils;

use fe::{lexer::Lexer, parser::parse_lexer, stmt::Stmt};
use mw::middleware;
use transpiler_cpp::transpiler;
use compiler_felf64::compiler::compiler;

fn cli() -> Command {
    Command::new("kappa")
        .about("Programming language made just for fun")
        .subcommand_required(true)
        .arg_required_else_help(true)
        .allow_external_subcommands(true)
        .subcommand(
            Command::new("compile")
                .about("Compile source code to target(s)")
                .arg_required_else_help(true)
                .arg(Arg::new("cpp")
                    .long("cpp")
                    .required(false)
                    .action(ArgAction::SetTrue)
                    .help("Target C++"))
                .arg(Arg::new("felf64")
                    .long("felf64")
                    .required(false)
                    .action(ArgAction::SetTrue)
                    .help("Target felf64"))
                .group(ArgGroup::new("targets")
                    .args(["cpp", "felf64"])
                    .required(true)
                    .multiple(true))
                .arg(Arg::new("filepath")
                    .short('f')
                    .long("filepath")
                    .required(true)
                    .action(ArgAction::Append)
                    .help("Source file path"))
    )
}

fn get_content(filepath: &str) -> Vec<String> {
    BufReader::new(std::fs::File::open(filepath)
        .expect("could not open file")).lines()
        .map(|line| line.expect("could not parse line"))
        .map(|line| line.replace("\t", "    ")) 
        .collect()
}

fn main() {
    let match_result = cli().get_matches();
    let compile_args = match_result.subcommand_matches("compile");
    let filepath: String = compile_args.unwrap()
        .get_one::<String>("filepath").unwrap().to_string();
    let cpp: bool = *compile_args.unwrap().get_one::<bool>("cpp").unwrap();
    let felf: bool = *compile_args.unwrap().get_one::<bool>("felf64").unwrap();
    let content: Vec<String> = get_content(&filepath);
    let mut lexer: Lexer = Lexer::new(content, filepath.to_string());
    lexer.print();
    let mut stmts: Vec<Stmt> = parse_lexer(&mut lexer);
    println!("");
    for stmt in stmts.iter() {
        println!("{}", stmt);
    }
    println!("");
    middleware(&mut stmts);
	for stmt in stmts.iter() {
        println!("{}", stmt);
    }
    println!("");
	let filename = filepath[..filepath.len()-1].to_string();
    if cpp {transpiler(filename.clone()+&"cpp", &stmts);}
    if felf {compiler(filename.clone()+&"asm", &stmts);}
}
