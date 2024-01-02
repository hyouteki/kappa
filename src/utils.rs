use std::process::exit;

pub fn error(message: String) {
    println!("error: {}", message);
    exit(1);
}

pub fn assert(eval: bool, message: String) {
    if !eval {error(message);} 
}
