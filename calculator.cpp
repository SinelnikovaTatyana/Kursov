/** @file calculator.cpp
 * @author Синельникова Т.А.
 * @version 1.0
 * @date 05.12.2024
 * @copyright ИБСТ ПГУ
 * @brief Реализация функций для вычисления произведения элементов вектора.
 *
 * Этот файл содержит реализацию функции, которая вычисляет произведение элементов вектора.
 */
#include "server.h"

#include <vector>
#include <limits>
#include <cstdint>

double Calculator::processVector(const std::vector<double>& vectorValues) {
    double product = 1.0;

    for (double value : vectorValues) {
        product *= value;
    }

    return product;
}
