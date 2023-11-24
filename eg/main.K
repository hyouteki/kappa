# fun ret(): any {
#     return true;
# }
#
# var n: int = 0;
# val s: str = "string";
# var mut_str: str = "hey";
#
# n = 4;
#
# # main function
# fun main(argc: int, string: str = "moto"): str { #hey
#     var x: int = 3;
#     if (ret()) {
#         print(string, x, "\n");
#     } else {
#         print(n, "\n");
#     }
#     return "hello";
# }
#
# print("If start\n");
# main(1);
# var x: str = main(0, main(0));
# x = main(0);
# print(x, "\n");
# x = "";
# print(x, "\n");
# print("If end\n");


var x: int = 0;

x = 1 + 2;

x = x + 1;

fun hello(): null {
    var y: int = 3 + 4;
    y = 2 + y;
    print(y, "\n");
}

hello();

print(x, "\n");
