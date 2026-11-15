
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>

#define T_LOW 72.0
#define T_HIGH 88.0
#define T_CENTER 80.0
#define Q_LOW_OFFSET 10.0
#define Q_MID_OFFSET 45.0
#define Q_HIGH_OFFSET 55.0
#define LOW_FACTOR 2.0
#define MID_FACTOR 3.0
#define HIGH_FACTOR 3.0
#define DIVISOR 3.0
#define SQRT_OFFSET 1.0

double calculateQ(double x);
int control(double q);
void printState(int state);
void calculateStatistics(const double q[], int n);

int main(void)
{
    double x;
    double q;
    double qValues[10];
    int state;
    int n = 0;
    FILE *in;
    printf("Программа мониторинга реактора.\n");
    printf("Преобразование температуры X в показатель Q.\n\n");
    in = fopen("input.txt", "rt");
    if (in == NULL)
    {
        printf("Ошибка открытия файла input.txt\n");
        return 1;
    }
    
    while (n < 10 && fscanf(in, "%lf", &x) == 1)
    {
        if (x < 60 || x > 100)
        {
            printf("Температура %.2f вне допустимого диапазона\n", x);
            continue;
        }
    
        q = calculateQ(x);
        printf("\nТемпература X = %.2f\n", x);
        printf("Показатель Q = %.2f\n", q);
        state = control(q);
        printState(state);
        qValues[n] = q;
        n++;
    }

    fclose(in);
    printf("\nОбработано показаний: %d\n", n);
    calculateStatistics(qValues, n);
    return 0;
}

double calculateQ(double x)
{
    if (x < T_LOW)
    {
        return LOW_FACTOR * (x - T_LOW) + Q_LOW_OFFSET;
    }
    else if (x <= T_HIGH)
    {
        return (x - T_CENTER) * (x - T_CENTER) / DIVISOR
            + Q_MID_OFFSET;
    }
    else
    {
        return HIGH_FACTOR * sqrt(x - T_HIGH + SQRT_OFFSET)
            + Q_HIGH_OFFSET;
    }
}

int control(double q)
{
    (void)q;
    // Будет реализована на следующем этапе
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
