#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
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
int control(double x, double q, double previousQ, int first, double dqMax);
void printState(int state);
void calculateStatistics(const double q[], int n);
void processFile(const char filename[], double dqMax);

int main(void)
{
    char filename[256] = "input.txt";
    char buffer[256];
    char extra;
    double dqMax = DQ_MAX;
    double newDQ;
    int choice;
    do
    {
        printf("\n===== МОНИТОРИНГ РЕАКТОРА =====\n");
        printf("1. Обработать файл измерений\n");
        printf("2. Изменить имя входного файла\n");
        printf("3. Изменить порог сигнализации\n");
        printf("4. Показать текущие настройки\n");
        printf("0. Выход\n");
        printf("Ваш выбор: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            break;
        }
        if (sscanf(buffer, "%d %c", &choice, &extra) != 1)
        {
            printf("Ошибка ввода\n");
            choice = -1;
            continue;
        }
        switch (choice)
        {
            case 1:
                processFile(filename, dqMax);
                break;
            case 2:
                printf("Введите имя файла: ");
                if (fgets(buffer, sizeof(buffer), stdin) != NULL)
                {
                    buffer[strcspn(buffer, "\n")] = '\0';
                    if (strlen(buffer) > 0)
                    {
                        strcpy(filename, buffer);
                        printf("Имя файла изменено\n");
                    }
                    else
                    {
                        printf("Ошибка имени файла\n");
                    }
                }
                break;
            case 3:
                printf("Введите новый порог: ");
                if (fgets(buffer, sizeof(buffer), stdin) != NULL)
                {
                    if (sscanf(buffer, "%lf %c", &newDQ, &extra) == 1
                        && isfinite(newDQ) && newDQ > 0)
                    {
                        dqMax = newDQ;
                        printf("Порог изменён\n");
                    }
                    else
                    {
                        printf("Некорректное значение порога\n");
                    }
                }
                break;
            case 4:
                printf("\n===== НАСТРОЙКИ =====\n");
                printf("Входной файл: %s\n", filename);
                printf("Порог сигнализации: %.2f\n", dqMax);
                printf("Диапазон температуры: %.2f - %.2f\n",
                       T_MIN, T_MAX);
                printf("Нормальный диапазон: %.2f - %.2f\n",
                       T_LOW, T_HIGH);
                break;
            case 0:
                printf("Завершение программы\n");
                break;
            default:
                printf("Неверный пункт меню\n");
                break;
        }
    } while (choice != 0);
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
        return (x - T_CENTER) * (x - T_CENTER)
            / DIVISOR + Q_MID_OFFSET;
    }
    else
    {
        return HIGH_FACTOR * sqrt(x - T_HIGH + SQRT_OFFSET)
            + Q_HIGH_OFFSET;
    }
}

int control(double x, double q, double previousQ,
            int first, double dqMax)
{
    if (!first && fabs(q - previousQ) > dqMax)
    {
        return 2;
    }
    else if (x < T_LOW || x > T_HIGH)
    {
        return 1;
    }
    else
    {
        return 0;
    }
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

void processFile(const char filename[], double dqMax)
{
    FILE *in;
    double x;
    double q;
    double previousQ = 0.0;
    double qValues[MAX_MEASUREMENTS];

    int first = 1;
    int state;
    int n = 0;
    int errors = 0;
    int record = 0;
    char buffer[256];
    char *end;
    in = fopen(filename, "rt");
    if (in == NULL)
    {
        printf("Ошибка открытия файла %s\n", filename);
        return;
    }
    printf("\n===== ОБРАБОТКА ФАЙЛА =====\n");
    while (fscanf(in, "%255s", buffer) == 1)
    {
        record++;
        x = strtod(buffer, &end);
        if (end == buffer || *end != '\0' || !isfinite(x))
        {
            printf("Запись №%d: некорректные данные\n", record);
            errors++;
            continue;
        }
        if (x < T_MIN || x > T_MAX)
        {
            printf("Запись №%d: температура вне диапазона\n",
                   record);
            errors++;
            continue;
        }
        q = calculateQ(x);
        state = control(x, q, previousQ, first, dqMax);
        printf("\nИзмерение №%d\n", record);
        printf("Температура X = %.2f\n", x);
        printf("Показатель Q = %.2f\n", q);

        if (!first)
        {
            printf("Изменение Q = %.2f\n",
                   fabs(q - previousQ));
        }
        printState(state);
        if (n < MAX_MEASUREMENTS)
        {
            qValues[n] = q;
        }
        n++;
        previousQ = q;
        first = 0;
    }
    if (ferror(in))
    {
        printf("\nОшибка чтения файла\n");
    }
    else if (feof(in))
    {
        printf("\nДостигнут конец файла\n");
    }
    fclose(in);
    if (record == 0)
    {
        printf("Файл пуст\n");
        return;
    }
    printf("\n===== РЕЗУЛЬТАТЫ ОБРАБОТКИ =====\n");
    printf("Всего записей: %d\n", record);
    printf("Обработано измерений: %d\n", n);
    printf("Ошибочных записей: %d\n", errors);
    if (n != MAX_MEASUREMENTS)
    {
        printf("Внимание: требуется 50 показаний\n");
    }
    calculateStatistics(qValues, n);
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
        printf("\nНедостаточно данных для статистики\n");
        printf("Необходимо минимум %d показаний\n",
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
