fun ret(): any {
    return true;
}

val n: int = 0;
val s: str = "string";
var mut_str: str = "hey";

# main function
fun main(argc, argv): str { #hey
    if (ret()) {
        print("Hello moto\n");
    } else {
        print("Hey there\n");
    }
    return "hello";
}

print("If start\n");
main();
print("If end\n");
