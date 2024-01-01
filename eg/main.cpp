#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
int fib(int n)
{
if ((n<0))
{
std::cout << "Invalid\n";
return (0-1);
}
if ((n<2))
{
return n;
}
return (fib((n-1))+fib((n-2)));
}
int main()
{
std::cout << fib(4) << "\n";
return 0;
}
