#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>

double calculateQ(double x)
{
    if (x < 72)
    {
        return 2 * (x - 72) + 10;
    }
    else if (x <= 88)
    {
        return (x - 80) * (x - 80) / 3.0 + 45;
    }
    else
    {
        return 3 * sqrt(x - 88 + 1) + 55;
    }
}
int main()
{
    double x;
    double q;
    printf("Введите температуру реактора X: ");
    scanf("%lf", &x);
    q = calculateQ(x);
    printf("Q = %.2f\n", q);
    return 0;
}
