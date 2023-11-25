fun factorial(num: int): int {
    if (num <= 1) {
        return num;
    }
    return factorial(num)+factorial(num-1);
}

print(factorial(4), "\n");
