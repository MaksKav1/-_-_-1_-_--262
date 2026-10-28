#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>

double readTemperature(void);
double calculateQ(double x);
int control(double q);
void printState(int state);
void calculateStatistics(const double q[], int n);

int main(void)
{
    double x;
    double q;
    int state;
    x = readTemperature();
    q = calculateQ(x);
    printf("Q = %.2f\n", q);
    state = control(q);
    printState(state);
    double qValues[1] = { q };
    calculateStatistics(qValues, 1);
    return 0;
}

double readTemperature(void)
{
    double x;
    printf("Введите температуру реактора X: ");
    scanf("%lf", &x);
    return x;
}

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

int control(double q)
{
    (void)q;
    return 0;
}

void printState(int state)
{
    (void)state;

    printf("STATE: функция будет реализована на следующем этапе\n");
}

void calculateStatistics(const double q[], int n)
{
    (void)q;
    (void)n;
    printf("STATISTICS: функция будет реализована на следующем этапе\n");
}
