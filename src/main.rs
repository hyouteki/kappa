use std::{io::{prelude::*, BufReader}};
use clap::{Arg, Command, ArgAction, ArgGroup};

pub mod fe;
pub mod transpiler_cpp;
pub mod compiler_felf64;

// TODO: make new module just for compiler
pub mod native_fun;

use fe::{lexer::Lexer, parser::parse_lexer, stmt::Stmt};
use transpiler_cpp::transpiler;
use compiler_felf64::{compiler};

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
    let stmts: Vec<Stmt> = parse_lexer(&mut lexer);
    println!("");
    for stmt in stmts.iter() {
        println!("{}", stmt);
    }
    println!("");
    // TODO: Add names
    if cpp {transpiler(String::from("eg/main.cpp"), &stmts);}
    if felf {compiler(String::from("eg/asm.asm"), &stmts);}
}
