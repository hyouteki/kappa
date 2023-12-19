#include <string>
#include <iostream>
int foo(int n)
{
return (n+(n-1));
}
int main()
{
std::cout << foo(4) << "\n";
return 0;
}
