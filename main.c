
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>

#define T_MIN 60.0
#define T_MAX 100.0
#define T_LOW 72.0
#define T_HIGH 88.0
#define T_CENTER 80.0
#define Q0 45.0
#define DQ_MAX 7.0
#define Q_LOW_OFFSET 10.0
#define Q_MID_OFFSET 45.0
#define Q_HIGH_OFFSET 55.0
#define LOW_FACTOR 2.0
#define HIGH_FACTOR 3.0
#define DIVISOR 3.0
#define SQRT_OFFSET 1.0
#define MAX_MEASUREMENTS 50
#define STAT_START 6
#define STAT_END 17

double calculateQ(double x);
int control(double x, double q);
void printState(int state);
void calculateStatistics(const double q[], int n);

int main(void)
{
    FILE *in;
    double x;
    double q;
    double qValues[MAX_MEASUREMENTS];
    int state;
    int n = 0;
    printf("Программа мониторинга реактора.\n");
    printf("Преобразование температуры X в показатель Q.\n\n");
    in = fopen("input.txt", "rt");
    if (in == NULL)
    {
        printf("Ошибка открытия файла input.txt\n");
        return 1;
    }
   
    while (n < MAX_MEASUREMENTS && fscanf(in, "%lf", &x) == 1)
    {
        printf("Температура X = %.2f\n", x);
        if (x < T_MIN || x > T_MAX)
        {
            printf("Ошибка: температура вне диапазона 60-100\n\n");
            continue;
        }
        q = calculateQ(x);
        printf("Показатель Q = %.2f\n", q);
        state = control(x, q);
        printState(state);
        qValues[n] = q;
        n++;
        printf("\n");
    }
    fclose(in);
    printf("Обработано показаний: %d\n", n);
    if (n != MAX_MEASUREMENTS)
    {
        printf("Внимание: для полного задания требуется 50 показаний.\n");
    }
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

int control(double x, double q)
{
    static double previousQ = 0.0;
    static int first = 1;
    int state;
    if (!first && fabs(q - previousQ) > DQ_MAX)
    {
        state = 2;
    }
    else if (x < T_LOW || x > T_HIGH)
    {
        state = 1;
    }
    else
    {
        state = 0;
    }
    previousQ = q;
    first = 0;
    return state;
}

void printState(int state)
{
    switch (state)
    {
        case 0:
            printf("STATE: НОРМА\n");
            break;
        case 1:
            printf("STATE: ПРЕДУПРЕЖДЕНИЕ\n");
            break;
        case 2:
            printf("STATE: ТРЕВОГА\n");
            break;
        default:
            printf("STATE: НЕИЗВЕСТНО\n");
            break;
    }
}

void calculateStatistics(const double q[], int n)
{
    double sum = 0.0;
    double average;
    double minimum;
    double maximum;
    double range;
    int count = 0;
    if (n <= STAT_END)
    {
        printf("\nНедостаточно данных для статистики.\n");
        printf("Необходимо минимум %d показаний.\n",
               STAT_END + 1);
        return;
    }
    minimum = q[STAT_START];
    maximum = q[STAT_START];
    for (int i = STAT_START; i <= STAT_END; i++)
    {
        sum += q[i];
        count++;
        if (q[i] < minimum)
        {
            minimum = q[i];
        }
        if (q[i] > maximum)
        {
            maximum = q[i];
        }
    }
    average = sum / count;
    range = maximum - minimum;
    printf("\n===== СТАТИСТИЧЕСКИЙ АНАЛИЗ =====\n");
    printf("Интервал: Q[%d..%d]\n", STAT_START, STAT_END);
    printf("Количество значений: %d\n", count);
    printf("Среднее значение: %.2f\n", average);
    printf("Минимальное значение: %.2f\n", minimum);
    printf("Максимальное значение: %.2f\n", maximum);
    printf("Размах: %.2f\n", range);
}
