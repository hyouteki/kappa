var num: int = 5;
while (num > 0) {
    num = num - 1;
    if (num == 4) {
        continue;
    }
    if (num == 3) {
        break;
    } 
    print("running\n");
}
exit(num);
