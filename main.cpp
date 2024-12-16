/** @file main.cpp
 * @author Синельникова Т.А.
 * @version 1.0
 * @date 05.12.2024
 * @copyright ИБСТ ПГУ
 * @brief Основной файл для запуска сервера.
 *
 * Этот файл содержит функцию main, которая отвечает за запуск сервера, 
 * обработку аргументов командной строки, настройку сервера и обработку векторов.
 */
#include "server.h"
#include <iostream>
#include <stdexcept>
#include <vector>

int main(int argc, char *argv[]) {
    try {
        Interface ui;
        if (ui.parseArguments(argc,argv) == 1) {
        std::string fileName = ui.getFileName(); 
        int port = ui.getPort();
        std::string file_error = ui.getErrorFile();
       
        Error errors;
        // Проверяем, что файл не открыт и не используются значения по умолчанию
        if (fileName != "/etc/vcalc.conf" || file_error != "/var/log/vcalc.log") {
            if (errors.er(fileName, file_error) == 12) { 
                throw RuntimeError("Ошибка открытия файла: " + fileName); 
            }
        }

        Server server(errors);
        Authorized authorized(errors);
        std::string error;
        int s = server.self_addr(error, file_error, port);
        if (s == -1) {
            throw RuntimeError("Ошибка при настройке сервера: " + error);
        }

        while (true) {
            int work_sock = server.client_addr(s, error, file_error);
            if (work_sock == -1) {
                continue; // Пропускаем итерацию, если произошла ошибка
            }

            if (authorized.authorized(work_sock, fileName, file_error) != 1) { 
                continue; // Пропускаем итерацию, если авторизация не пройдена
            }

            // Получаем данные от клиента
            uint32_t Quantity;
            uint32_t Length;
            double Vector_number;
            std::vector<double> vectorValues;

            ssize_t bytes_received = recv(work_sock, &Quantity, sizeof(Quantity), 0);
            if (bytes_received <= 0) {
                throw RuntimeError("Ошибка при получении количества векторов");
            }

            for (uint32_t j = 0; j < Quantity; j++) {
                bytes_received = recv(work_sock, &Length, sizeof(Length), 0);
                if (bytes_received <= 0) {
                    throw RuntimeError("Ошибка при получении длины вектора");
                }

                vectorValues.clear();
                for (uint32_t i = 0; i < Length; i++) {
                    bytes_received = recv(work_sock, &Vector_number, sizeof(Vector_number), 0);
                    if (bytes_received <= 0) {
                        throw RuntimeError("Ошибка при получении элемента вектора");
                    }
                    if (bytes_received != sizeof(double)) {
                        std::cerr << "Ошибка: количество байт не совпадает с размером типа double" << std::endl;
                        close(work_sock);
                        continue;
                    }
                    vectorValues.push_back(Vector_number);
                }

                // Вычисляем произведение вектора
                double product = Calculator::processVector(vectorValues);

                // Отправляем результат клиенту
                ssize_t bytes_sent = send(work_sock, &product, sizeof(product), 0);
                if (bytes_sent <= 0) {
                    throw RuntimeError("Ошибка при отправке результата");
                }
            }

            close(work_sock);
        }
        }
    } catch (const RuntimeError& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Непредвиденная ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
