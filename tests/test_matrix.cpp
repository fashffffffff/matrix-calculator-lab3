#include "matrix.h"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

namespace matrix {
namespace {

/** @brief Хранит матрицу до конца теста, в том числе при ASSERT. */
struct TestMatrix {
    Matrix value;

    /** @brief Принимает владение выделенной матрицей. @param m Исходная матрица. */
    explicit TestMatrix(Matrix m) : value(m) {}
    TestMatrix(const TestMatrix&) = delete;
    TestMatrix& operator=(const TestMatrix&) = delete;

    /** @brief Освобождает матрицу. */
    ~TestMatrix() { free_matrix(value); }
};

/** @brief Проверяет размеры и нулевую инициализацию. */
TEST(MatrixTest, CreatesZeroMatrix) {
    const TestMatrix result(create_matrix(3, 4));
    ASSERT_EQ(result.value.rows, 3);
    ASSERT_EQ(result.value.cols, 4);
    ASSERT_NE(result.value.data, nullptr);
    for (int row = 0; row < result.value.rows; ++row) {
        for (int col = 0; col < result.value.cols; ++col) {
            EXPECT_DOUBLE_EQ(result.value.data[row][col], 0.0);
        }
    }
}

/** @brief Проверяет независимое копирование построчно. */
TEST(MatrixTest, CopiesArrayInRowOrder) {
    double values[] = {1, 2, 3, 4, 5, 6};
    const TestMatrix result(matrix_from_array(values, 2, 3));
    ASSERT_EQ(result.value.rows, 2);
    ASSERT_EQ(result.value.cols, 3);
    values[0] = 99;
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 3; ++col) {
            EXPECT_DOUBLE_EQ(result.value.data[row][col], row * 3 + col + 1);
        }
    }
}

/** @brief Проверяет сложение двух матриц 2 × 2. */
TEST(MatrixTest, AddsMatrices) {
    double left[] = {1, 2, 3, 4};
    double right[] = {5, 6, 7, 8};
    const TestMatrix a(matrix_from_array(left, 2, 2));
    const TestMatrix b(matrix_from_array(right, 2, 2));
    const TestMatrix result(matrix_add(a.value, b.value));
    ASSERT_EQ(result.value.rows, 2);
    ASSERT_EQ(result.value.cols, 2);
    EXPECT_DOUBLE_EQ(result.value.data[0][0], 6);
    EXPECT_DOUBLE_EQ(result.value.data[0][1], 8);
    EXPECT_DOUBLE_EQ(result.value.data[1][0], 10);
    EXPECT_DOUBLE_EQ(result.value.data[1][1], 12);
}

/** @brief Проверяет умножение прямоугольных матриц. */
TEST(MatrixTest, MultipliesRectangularMatrices) {
    double left[] = {1, 2, 3, 4, 5, 6};
    double right[] = {7, 8, 9, 10, 11, 12};
    const TestMatrix a(matrix_from_array(left, 2, 3));
    const TestMatrix b(matrix_from_array(right, 3, 2));
    const TestMatrix result(matrix_multiply(a.value, b.value));
    ASSERT_EQ(result.value.rows, 2);
    ASSERT_EQ(result.value.cols, 2);
    EXPECT_DOUBLE_EQ(result.value.data[0][0], 58);
    EXPECT_DOUBLE_EQ(result.value.data[0][1], 64);
    EXPECT_DOUBLE_EQ(result.value.data[1][0], 139);
    EXPECT_DOUBLE_EQ(result.value.data[1][1], 154);
}

/** @brief Проверяет скалярное произведение. */
TEST(MatrixTest, MultipliesRowByColumn) {
    double left[] = {1, 2, 3};
    double right[] = {4, 5, 6};
    const TestMatrix a(matrix_from_array(left, 1, 3));
    const TestMatrix b(matrix_from_array(right, 3, 1));
    const TestMatrix result(matrix_multiply(a.value, b.value));
    ASSERT_EQ(result.value.rows, 1);
    ASSERT_EQ(result.value.cols, 1);
    EXPECT_DOUBLE_EQ(result.value.data[0][0], 32);
}

/** @brief Проверяет отказ при несовместимых размерах. */
TEST(MatrixTest, RejectsIncompatibleMatrices) {
    const TestMatrix a(create_matrix(2, 3));
    const TestMatrix b(create_matrix(2, 2));
    EXPECT_THROW(matrix_add(a.value, b.value), std::invalid_argument);
    EXPECT_THROW(matrix_multiply(a.value, b.value), std::invalid_argument);
}

/** @brief Проверяет отказ при пустом входе и неверных размерах. */
TEST(MatrixTest, RejectsInvalidInput) {
    const TestMatrix a(create_matrix(1, 1));
    double* empty_row[] = {nullptr};
    EXPECT_THROW(create_matrix(0, 2), std::invalid_argument);
    EXPECT_THROW(create_matrix(2, -1), std::invalid_argument);
    EXPECT_THROW(matrix_from_array(nullptr, 2, 2), std::invalid_argument);
    EXPECT_THROW(matrix_add({}, a.value), std::invalid_argument);
    EXPECT_THROW(matrix_multiply(a.value, (Matrix{empty_row, 1, 1})), std::invalid_argument);
}

/** @brief Проверяет обработку арифметического переполнения. */
TEST(MatrixTest, RejectsArithmeticOverflow) {
    double largest[] = {std::numeric_limits<double>::max()};
    double two[] = {2};
    const TestMatrix a(matrix_from_array(largest, 1, 1));
    const TestMatrix b(matrix_from_array(two, 1, 1));
    EXPECT_THROW(matrix_add(a.value, a.value), std::overflow_error);
    EXPECT_THROW(matrix_multiply(a.value, b.value), std::overflow_error);
}

/** @brief Проверяет безопасное освобождение пустой структуры. */
TEST(MatrixTest, FreesEmptyMatrix) {
    EXPECT_NO_THROW(free_matrix({}));
    EXPECT_NO_THROW(free_matrix({}));
}

/** @brief Проверяет обычный формат и сообщение о пустой матрице. */
TEST(MatrixTest, PrintsSmallAndEmptyMatrices) {
    double values[] = {1.25, -2.5};
    const TestMatrix a(matrix_from_array(values, 1, 2));
    testing::internal::CaptureStdout();
    print_matrix(a.value);
    print_matrix({});
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "[  1.25  -2.50]\n[Empty matrix]\n");
}

/** @brief Проверяет сокращение широкой матрицы с одной строкой. */
TEST(MatrixTest, PrintsWideMatrixWithLastColumns) {
    double values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    const TestMatrix a(matrix_from_array(values, 1, 10));
    testing::internal::CaptureStdout();
    print_matrix(a.value);
    const std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("1.00"), std::string::npos);
    EXPECT_NE(output.find("10.00"), std::string::npos);
    EXPECT_NE(output.find("2 columns omitted"), std::string::npos);
}

/** @brief Проверяет сокращение высокой матрицы с одним столбцом. */
TEST(MatrixTest, PrintsTallMatrixWithLastRows) {
    double values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    const TestMatrix a(matrix_from_array(values, 12, 1));
    testing::internal::CaptureStdout();
    print_matrix(a.value);
    const std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("1.00"), std::string::npos);
    EXPECT_NE(output.find("12.00"), std::string::npos);
    EXPECT_NE(output.find("2 rows omitted"), std::string::npos);
}

/** @brief Проверяет размерность и диапазон случайной матрицы. */
TEST(MatrixRandomTest, UsesRequestedShapeAndBounds) {
    const TestMatrix result(matrix_random(3, 5, -4.0, 2.5));
    ASSERT_EQ(result.value.rows, 3);
    ASSERT_EQ(result.value.cols, 5);
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 5; ++col) {
            EXPECT_GE(result.value.data[row][col], -4.0);
            EXPECT_LE(result.value.data[row][col], 2.5);
        }
    }
}

/** @brief Проверяет вырожденный интервал и минимальную матрицу. */
TEST(MatrixRandomTest, AcceptsEqualBounds) {
    const TestMatrix result(matrix_random(1, 1, -7.0, -7.0));
    ASSERT_EQ(result.value.rows, 1);
    ASSERT_EQ(result.value.cols, 1);
    EXPECT_DOUBLE_EQ(result.value.data[0][0], -7.0);
}

/** @brief Проверяет отказ при неверных границах и размере. */
TEST(MatrixRandomTest, RejectsInvalidArguments) {
    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();
    EXPECT_THROW(matrix_random(1, 0, 0, 1), std::invalid_argument);
    EXPECT_THROW(matrix_random(2, 2, 3, -3), std::invalid_argument);
    EXPECT_THROW(matrix_random(2, 2, 0, inf), std::invalid_argument);
    EXPECT_THROW(matrix_random(2, 2, nan, 1), std::invalid_argument);
}

/** @brief Проверяет интервал, ширина которого превышает double. */
TEST(MatrixRandomTest, HandlesExtremeFiniteBounds) {
    const double largest = std::numeric_limits<double>::max();
    const TestMatrix result(matrix_random(4, 5, -largest, largest));
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 5; ++col) {
            EXPECT_TRUE(std::isfinite(result.value.data[row][col]));
        }
    }
}

} // namespace
} // namespace matrix
