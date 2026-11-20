/** @file matrix.cpp
 *  @brief Реализация операций с динамическими матрицами.
 */
#include "matrix.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>

namespace matrix {
namespace {

constexpr int kVisibleRows = 10;
constexpr int kVisibleCols = 8;
constexpr int kFieldWidth = 6;
constexpr int kPrecision = 2;

/** @brief Проверяет размеры до выделения памяти.
 *  @param rows Число строк.
 *  @param cols Число столбцов.
 *  @exception std::invalid_argument Хотя бы один размер неположительный.
 */
void check_dimensions(int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument("Matrix dimensions must be positive");
    }
}

/** @brief Проверяет размеры и наличие всех строк.
 *  @param m Матрица.
 *  @return true, если размерность положительна и указатели заданы.
 */
bool is_valid(Matrix m) {
    if (m.data == nullptr || m.rows <= 0 || m.cols <= 0) {
        return false;
    }
    return std::all_of(m.data, m.data + m.rows, [](const double* row) { return row != nullptr; });
}

/** @brief Проверяет, что результат представим конечным числом.
 *  @param result Матрица, которую следует освободить при ошибке.
 *  @param value Результат вычисления.
 *  @return Проверенное значение.
 *  @exception std::overflow_error Результат бесконечен или NaN.
 */
double check_result(Matrix result, double value) {
    if (!std::isfinite(value)) {
        free_matrix(result);
        throw std::overflow_error("Matrix operation produced a non-finite result");
    }
    return value;
}

/** @brief Записывает одну строку, сохраняя видимость последних столбцов.
 *  @param output Поток с настроенной точностью.
 *  @param values Элементы строки.
 *  @param cols Полное число столбцов.
 */
void write_row(std::ostream& output, const double* values, int cols) {
    const int visible_cols = std::min(cols, kVisibleCols);
    output << '[';
    for (int col = 0; col < visible_cols; ++col) {
        if (col > 0) {
            output << ' ';
        }
        if (cols > kVisibleCols && col == kVisibleCols / 2) {
            output << "... ";
        }
        const int index = col < kVisibleCols / 2 ? col : col + cols - visible_cols;
        output << std::setw(kFieldWidth) << values[index];
    }
    output << "]\n";
}

/** @brief Масштабирует долю интервала, избегая переполнения разности.
 *  @param min Нижняя граница.
 *  @param max Верхняя граница.
 *  @param fraction Доля в диапазоне [0, 1].
 *  @return Значение в диапазоне [min, max].
 */
double interpolate(double min, double max, double fraction) {
    if (min < 0 && max > 0) {
        return (1 - fraction) * min + fraction * max;
    }
    return std::clamp(min + fraction * (max - min), min, max);
}

} // namespace

Matrix create_matrix(int rows, int cols) {
    check_dimensions(rows, cols);
    Matrix result{new double*[rows]{}, rows, cols};
    try {
        for (int row = 0; row < rows; ++row) {
            result.data[row] = new double[cols]{};
        }
    } catch (...) {
        free_matrix(result);
        throw;
    }
    return result;
}

void free_matrix(Matrix m) noexcept {
    if (m.data == nullptr) {
        return;
    }
    for (int row = 0; row < m.rows; ++row) {
        delete[] m.data[row];
    }
    delete[] m.data;
}

Matrix matrix_add(Matrix a, Matrix b) {
    if (!is_valid(a) || !is_valid(b) || a.rows != b.rows || a.cols != b.cols) {
        throw std::invalid_argument("Addition requires valid matrices of equal dimensions");
    }

    Matrix result = create_matrix(a.rows, a.cols);
    for (int row = 0; row < a.rows; ++row) {
        for (int col = 0; col < a.cols; ++col) {
            result.data[row][col] = check_result(result, a.data[row][col] + b.data[row][col]);
        }
    }
    return result;
}

Matrix matrix_multiply(Matrix a, Matrix b) {
    if (!is_valid(a) || !is_valid(b) || a.cols != b.rows) {
        throw std::invalid_argument("Multiplication requires valid matrices with a.cols == b.rows");
    }

    Matrix result = create_matrix(a.rows, b.cols);
    for (int row = 0; row < a.rows; ++row) {
        for (int col = 0; col < b.cols; ++col) {
            double sum = 0.0;
            for (int inner = 0; inner < a.cols; ++inner) {
                sum += a.data[row][inner] * b.data[inner][col];
            }
            result.data[row][col] = check_result(result, sum);
        }
    }
    return result;
}

void print_matrix(Matrix m) {
    if (!is_valid(m)) {
        std::cout << "[Empty matrix]\n";
        return;
    }

    std::ostringstream output;
    output << std::fixed << std::setprecision(kPrecision);
    const int visible_rows = std::min(m.rows, kVisibleRows);
    for (int row = 0; row < visible_rows; ++row) {
        if (m.rows > kVisibleRows && row == kVisibleRows / 2) {
            output << "[...]\n";
        }
        const int index = row < kVisibleRows / 2 ? row : row + m.rows - visible_rows;
        write_row(output, m.data[index], m.cols);
    }
    if (m.rows > kVisibleRows || m.cols > kVisibleCols) {
        output << "Matrix " << m.rows << 'x' << m.cols << " ("
               << m.rows - visible_rows << " rows omitted, "
               << std::max(0, m.cols - kVisibleCols) << " columns omitted)\n";
    }
    std::cout << output.str();
}

Matrix matrix_from_array(double* data, int rows, int cols) {
    if (data == nullptr) {
        throw std::invalid_argument("Source array must not be null");
    }

    Matrix result = create_matrix(rows, cols);
    for (int row = 0; row < rows; ++row) {
        const auto offset = static_cast<std::size_t>(row) * static_cast<std::size_t>(cols);
        std::copy_n(data + offset, cols, result.data[row]);
    }
    return result;
}

Matrix matrix_random(int rows, int cols, double min, double max) {
    check_dimensions(rows, cols);
    if (!std::isfinite(min) || !std::isfinite(max) || min > max) {
        throw std::invalid_argument("Random bounds must be finite and ordered");
    }

    if (min == max) {
        Matrix result = create_matrix(rows, cols);
        for (int row = 0; row < rows; ++row) {
            std::fill_n(result.data[row], cols, min);
        }
        return result;
    }

    std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<double> fraction(0.0, 1.0);
    Matrix result = create_matrix(rows, cols);
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            result.data[row][col] = interpolate(min, max, fraction(generator));
        }
    }
    return result;
}

} // namespace matrix
