/** @file main.cpp
 *  @brief Демонстрация базовых операций и генерации матрицы.
 */
#include "matrix.h"

#include <exception>
#include <iostream>

namespace matrix {
namespace {

/** @brief Владеет результатом операции до выхода из текущей области видимости. */
struct DemoMatrix {
    Matrix value;

    /** @brief Принимает владение матрицей. @param m Исходная матрица. */
    explicit DemoMatrix(Matrix m) : value(m) {}
    DemoMatrix(const DemoMatrix&) = delete;
    DemoMatrix& operator=(const DemoMatrix&) = delete;

    /** @brief Освобождает матрицу. */
    ~DemoMatrix() { free_matrix(value); }
};

/** @brief Печатает название и значения матрицы.
 *  @param label Название матрицы.
 *  @param value Матрица.
 */
void show(const char* label, Matrix value) {
    std::cout << label << '\n';
    print_matrix(value);
}

/** @brief Демонстрирует вычисления и вариант № 24. */
void run_demo() {
    double left[] = {1, 2, 3, 4};
    double right[] = {5, 6, 7, 8};
    const DemoMatrix a(matrix_from_array(left, 2, 2));
    const DemoMatrix b(matrix_from_array(right, 2, 2));
    const DemoMatrix sum(matrix_add(a.value, b.value));
    const DemoMatrix product(matrix_multiply(a.value, b.value));
    const DemoMatrix random(matrix_random(3, 4, -5.0, 5.0));

    show("Matrix A", a.value);
    show("Matrix B", b.value);
    show("A + B", sum.value);
    show("A * B", product.value);
    show("Random matrix 3x4 [-5, 5] (variant 24)", random.value);
}

} // namespace
} // namespace matrix

/** @brief Запускает демонстрацию.
 *  @return 0 при успехе, 1 при ошибке.
 */
int main() {
    try {
        matrix::run_demo();
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Matrix calculator error: " << error.what() << '\n';
        return 1;
    }
}
