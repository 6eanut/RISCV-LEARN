#include <stdio.h>
#include <stdint.h>
int main()
{
    int a = 0, b = 10, c = 2;
    for (int i = 1; i < 10; i++)
    {
        a += b;
        b -= c;
        c += a;
    }
    return 0;
}