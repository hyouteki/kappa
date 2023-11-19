fun ret() {
    return true;
}

fun main(argc, argv) {
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
