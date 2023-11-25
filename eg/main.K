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


# var x: int = 0;
#
# x = 5 ** 2;
# x = x * 3;
#
# print("and =", true && false, "\n");
# print("or  =", true || false, "\n");
#
# print("4 & 5 = ", 4 & 5, "\n");
# print("4 | 5 = ", 4 | 5, "\n");
# print("4 ^ 5 = ", 4 ^ 5, "\n");
#
# val z: str = "kap" + "pa";
# print(z, "\n");
#
# fun hello(): null {
#     var y: int = 3 + 4;
#     y = 2 + y;
#     print(y, "\n");
# }
#
# hello();
#
# print(x, "\n");

var x: int = 1;

while (x <= 5) {
    print(x, "\n");
    x = x + 1;
}
