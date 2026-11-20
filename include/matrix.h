/** @file matrix.h
 *  @brief Интерфейс библиотеки матричных операций.
 */
#pragma once

namespace matrix {

/** @brief Матрица с динамически выделенными строками.
 *  @warning Копирование структуры копирует только указатель. Освобождайте
 *  каждую выделенную матрицу ровно один раз через free_matrix().
 */
struct Matrix {
    double** data = nullptr; ///< Указатели на строки матрицы.
    int rows = 0;            ///< Число строк.
    int cols = 0;            ///< Число столбцов.
};

/** @brief Выделяет матрицу и заполняет её нулями.
 *  @param rows Число строк.
 *  @param cols Число столбцов.
 *  @return Новая матрица, которую нужно освободить.
 *  @exception std::invalid_argument Размеры должны быть положительными.
 *  @exception std::bad_alloc Не удалось выделить память.
 */
Matrix create_matrix(int rows, int cols);

/** @brief Освобождает строки и массив указателей.
 *  @param m Владеющая матрица или пустая структура.
 *  @warning После вызова указатель в исходной структуре становится висячим.
 *  @note Пустую матрицу можно передавать повторно.
 */
void free_matrix(Matrix m) noexcept;

/** @brief Складывает две матрицы поэлементно.
 *  @param a Первое слагаемое.
 *  @param b Второе слагаемое той же размерности.
 *  @return Новая матрица суммы.
 *  @exception std::invalid_argument Некорректная матрица или разные размеры.
 *  @exception std::overflow_error Получен неконечный арифметический результат.
 *  @exception std::bad_alloc Не удалось выделить память.
 */
Matrix matrix_add(Matrix a, Matrix b);

/** @brief Умножает матрицу M × N на матрицу N × K.
 *  @param a Левый множитель.
 *  @param b Правый множитель.
 *  @return Новая матрица M × K.
 *  @exception std::invalid_argument Некорректная матрица или несовместимые размеры.
 *  @exception std::overflow_error Получен неконечный арифметический результат.
 *  @exception std::bad_alloc Не удалось выделить память.
 *  @note Время выполнения O(M × N × K).
 */
Matrix matrix_multiply(Matrix a, Matrix b);

/** @brief Выводит матрицу с двумя знаками после точки и сокращает большие матрицы.
 *  @param m Матрица для вывода. Для пустой выводится [Empty matrix].
 */
void print_matrix(Matrix m);

/** @brief Копирует плоский массив в новую матрицу построчно.
 *  @param data Указатель на массив длиной не менее rows × cols.
 *  @param rows Число строк.
 *  @param cols Число столбцов.
 *  @return Новая матрица с копией данных.
 *  @exception std::invalid_argument Нулевой указатель или неположительный размер.
 *  @exception std::bad_alloc Не удалось выделить память.
 */
Matrix matrix_from_array(double* data, int rows, int cols);

/** @brief Создаёт матрицу с равномерно распределёнными случайными числами.
 *  @param rows Число строк.
 *  @param cols Число столбцов.
 *  @param min Нижняя конечная граница.
 *  @param max Верхняя конечная граница.
 *  @return Новая матрица со значениями в [min, max]. При min == max заполняется константой.
 *  @exception std::invalid_argument Неположительный размер или неверные границы.
 *  @exception std::bad_alloc Не удалось выделить память.
 *  @exception std::runtime_error Системный источник случайности недоступен.
 */
Matrix matrix_random(int rows, int cols, double min, double max);

} // namespace matrix
