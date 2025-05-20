#include <stdio.h>
int main()
{
       int add_sum = 0;
       for (int i = 0; i < 10; i++)
       {
              add_sum += i;
       }
       printf("add_sum = %d\n", add_sum);

       int mul_sum = 1;
       for (int i = 1; i <= 10; i++)
       {
              mul_sum += i;
       }
       printf("mul_sum = %d\n", mul_sum);
       return 0;
}