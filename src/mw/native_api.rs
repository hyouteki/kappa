use std::collections::HashMap;
use crate::mw::validator::Fun;
use crate::fe::stmt::{Arg, Type};

pub fn get_native_apis() -> HashMap<String, Fun> {
    HashMap::from([
        (
            String::from("exit"),
            Fun::new(vec![
                Arg::new(String::from("error_code"), Type::Int)
            ], Type::Int) // TODO: for now it is Int add a new type Void and change this to void 
        ),
        (
            String::from("print"),
            Fun::new(vec![
                Arg::new(String::from("text"), Type::Str)
            ], Type::Int) // TODO: change it to void 
        )
    ])
}
