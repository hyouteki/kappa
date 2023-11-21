fun ret(): any {
    return true;
}

val n: int = 0;
val s: str = "string";
var mut_str: str = "hey";

# main function
fun main(argc: int, string: str = "moto"): str { #hey
    var x: int = 3;
    if (ret()) {
        print(string, x, "\n");
    } else {
        print(n, "\n");
    }
    return "hello";
}

print("If start\n");
main(1);
val x: str = main(0, main(0));
print(x, "\n");
print("If end\n");
