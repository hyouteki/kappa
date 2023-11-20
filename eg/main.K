fun ret(): any {
    return true;
}

val n: int = 0;
val s: str = "string";
var mut_str: str = "hey";

# main function
fun main(argc: int, string: str = "hello"): str { #hey
    if (ret()) {
        print(string, "\n");
    } else {
        print(n, "\n");
    }
    return "hello";
}

print("If start\n");
main(1);
print("If end\n");
