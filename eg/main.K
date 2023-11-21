fun ret(): any {
    return true;
}

var n: int = 0;
val s: str = "string";
var mut_str: str = "hey";

n = 4;

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
var x: str = main(0, main(0));
print(x, "\n");
x = "";
print(x, "\n");
print("If end\n");
