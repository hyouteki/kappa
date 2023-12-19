fn foo(n: int): int {
	if (true) {
		return 1;
	} 
	return n+(n-1);
}

fn main(): int {
	print(foo(4), "\n");
	return 0;
}
