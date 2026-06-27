#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <locale>
#include <windows.h>

constexpr double EPSILON_COARSE = 0.1;
constexpr double EPSILON_FINE   = 0.000001;

double series(double x, double epsilon, int& numTerms);
double func(double x);
void clearConsole();

// -----------------------------------------------------------------------
// Валидация ввода: xn < xk, dx > 0, как минимум одно приращение
// -----------------------------------------------------------------------
bool isValidInput(double xn, double xk, double dx) {
    return (xn < xk) && (dx > 0) && ((xk - xn) / dx >= 1);
}

// -----------------------------------------------------------------------
// Очистка консоли
// -----------------------------------------------------------------------
void clearConsole() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

// -----------------------------------------------------------------------
// Точка входа
// -----------------------------------------------------------------------
int main() {
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    char choice;

    do {
        clearConsole();

        double xn = 0, xk = 0, dx = 0;

        std::cout << "TaylorE v1.0.0 — Разложение e^(-x) в ряд Тейлора\n"
                  << std::endl;

        // ── Цикл ввода с валидацией ──────────────────────────────────
        while (true) {
            std::cout << "Введите начальное значение x (xn): ";
            if (!(std::cin >> xn)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                clearConsole();
                std::cout << "Ошибка: введите число.\n";
                continue;
            }

            std::cout << "Введите конечное значение x (xk): ";
            if (!(std::cin >> xk)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                clearConsole();
                std::cout << "Ошибка: введите число.\n";
                continue;
            }

            std::cout << "Введите шаг изменения x (dx): ";
            if (!(std::cin >> dx)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                clearConsole();
                std::cout << "Ошибка: введите число.\n";
                continue;
            }

            if (isValidInput(xn, xk, dx)) {
                break;
            }

            clearConsole();
            std::cout << "Ошибка: неверные значения xn, xk или dx. "
                         "Убедитесь, что xn < xk и dx > 0.\n";
        }

        // ── Расчёт ───────────────────────────────────────────────────
        double totalRate1 = 0, totalRate2 = 0;
        int numPoints1 = 0, numPoints2 = 0;

        std::cout << " _______________________________________________________________________________\n"
                  << " |    x   |  eps1 |   f1   |  rate1  |    eps2    |   f2   |  rate2  |   f(x)  |\n"
                  << " |        |  0.1  |        |         |  0.000001  |        |         |         |\n"
                  << " |--------|-------|--------|---------|------------|--------|---------|---------|\n";

        for (double x = xn; x <= xk; x += dx) {
            int numTerms1 = 0, numTerms2 = 0;
            double fx = func(x);

            double result1 = series(x, EPSILON_COARSE, numTerms1);
            double result2 = series(x, EPSILON_FINE,   numTerms2);

            double rate1 = (result1 > 0)
                ? std::fabs(std::log(result1) / std::log(static_cast<double>(numTerms1)))
                : 0.0;
            double rate2 = (result2 > 0)
                ? std::fabs(std::log(result2) / std::log(static_cast<double>(numTerms2)))
                : 0.0;

            if (std::isnan(result1) || result1 <= 0) {
                result1 = std::numeric_limits<double>::quiet_NaN();
                rate1   = std::numeric_limits<double>::quiet_NaN();
            }
            if (std::isnan(result2) || result2 <= 0) {
                result2 = std::numeric_limits<double>::quiet_NaN();
                rate2   = std::numeric_limits<double>::quiet_NaN();
            }

            if (!std::isnan(rate1)) { totalRate1 += rate1; ++numPoints1; }
            if (!std::isnan(rate2)) { totalRate2 += rate2; ++numPoints2; }

            std::cout << std::fixed << std::setprecision(4)
                      << " | " << std::setw(4) << x
                      << " |       | " << std::setw(6) << result1
                      << " | " << std::setw(7) << std::setprecision(5) << rate1
                      << " |            | " << std::setw(6) << std::setprecision(4) << result2
                      << " | " << std::setw(7) << std::setprecision(5) << rate2
                      << " | " << std::setw(6) << fx
                      << " |\n";
        }

        double avgRate1 = (numPoints1 > 0) ? totalRate1 / numPoints1 : 0.0;
        double avgRate2 = (numPoints2 > 0) ? totalRate2 / numPoints2 : 0.0;

        std::cout << " |--------|-------|--------|---------|------------|--------|---------|---------|\n"
                  << " | Total  |       |        | " << std::setw(7) << std::setprecision(5) << avgRate1
                  << " |            |        | " << std::setw(7) << std::setprecision(5) << avgRate2
                  << " |         |\n"
                  << " -------------------------------------------------------------------------------\n";

        std::cout << "Хотите повторить ввод значений? (y/n): ";
        std::cin >> choice;
    } while (choice == 'y' || choice == 'Y');

    return 0;
}

// -----------------------------------------------------------------------
// Разложение e^(-x) в ряд Тейлора: Σ (-x)^k / k!
//   result = term₀ + term₁ + …  до тех пор, пока |termₖ| > epsilon
// -----------------------------------------------------------------------
double series(double x, double epsilon, int& numTerms) {
    double result = 0.0;
    double term = 1.0;
    int k = 0;
    numTerms = 0;

    while (std::fabs(term) > epsilon) {
        result += term;
        term *= (-x) / (k + 1);
        ++k;
        ++numTerms;
    }

    return result;
}

// -----------------------------------------------------------------------
// Эталонная функция — e^(-x) через стандартную библиотеку
// -----------------------------------------------------------------------
double func(double x) {
    return std::exp(-x);
}
