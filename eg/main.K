fn fib(n: int): int {
	if (n < 0) {
		print("Invalid\n");
		return 0-1;
	}
	if (n < 2) {
		return n;
	}
	return fib(n-1) + fib(n-2);
}

fn main(): int {
	print(fib(4), "\n");
	return 0;
}
