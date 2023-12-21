fn fib(n: int): int {
	if (n < 2) {
		return n;
	} 
	return fib(n-1) + fib(n-2);
}

fn main(): int {
	print(fib(100), "\n");
	return 0;
}
