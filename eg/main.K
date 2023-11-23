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


fun foo(): str {
    return "str";
}

var x: any = null;
print(x, "\n");
x = foo();
print(x, "\n");
x = 6;
print(x, "\n");
var y: str = null;
print(y, "\n");
y = "hmm";
print(y, "\n");
y = null;
print(y, "\n");
