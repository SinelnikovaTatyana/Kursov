/** @file Interface.cpp
 * @author Синельникова Т.А.
 * @version 1.0
 * @date 05.12.2024
 * @copyright ИБСТ ПГУ
 * @brief Реализация функций для обработки интерфейса командной строки.
 *
 * Этот файл содержит реализацию функций, связанных с разбором аргументов командной строки и 
 *предоставлением доступа к параметрам.
 */
#include "server.h"
#include <string>
#include <iostream>
#include <stdexcept>
#include <getopt.h>
#include <fstream>

using namespace std;

std::string Interface::getFileName() const {
    return fileName;
}

int Interface::getPort() const {
    return port;
}

std::string Interface::getErrorFile() const {
    return file_error;
}

std::string Interface::getDescription() const {
    return description;
}

bool Interface::parseArguments(int argc, char* argv[]) {
    const struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"file", required_argument, 0, 'f'},
        {"port", required_argument, 0, 'p'},
        {"error", required_argument, 0, 'e'},
        {0, 0, 0, 0}
    };

    int option = 0;
    int option_index = 0;

    while ((option = getopt_long(argc, argv, "hf:p:e:", long_options, &option_index)) != -1) {
        switch (option) {
           case 'h':
                description = "Векторный калькулятор выполняющий действие нахождения произведения векторов\n"
                              "Инструкция: -f База данных пользователей -p Порт -e Файл ошибок\n"
                              "-h Помощь\n"
                              "-f Название файла\n"
                              "-p Порт\n"
                              "-e Файл ошибок\n";
                std::cout << description;
                return false;
            case 'f': {
                fileName = std::string(optarg);
                if (fileName.empty()) {
                    throw RuntimeError("Название файла не может быть пустым");
                }
                std::ifstream file(fileName);
                if (!file.good()) {
                    throw RuntimeError("Файл не существует: " + fileName);
                }
            }
                break;
            case 'p': {
                int z = std::stoi(std::string(optarg));
                if (z < 1024 || z > 65535) {
                    throw RuntimeError("Неверный порт. Порт должен быть в диапазоне от 1024 до 65535.");
                    exit(1); // Завершаем программу с ошибкой
                }
                port = z;
            }
                break;
            case 'e': {
                file_error = std::string(optarg);
                if (file_error.empty()) {
                    throw RuntimeError("Название файла для ошибок не может быть пустым");
                }
                std::ifstream file(file_error);
                if (!file.good()) {
                    throw RuntimeError("Файл для ошибок не существует: " + file_error);
                }
            }
                break;
            case '?':
                throw RuntimeError("Неверно введен параметр. Используйте -h для помощи.");
            default:
                throw RuntimeError("Неизвестный параметр. Используйте -h для помощи.");
        }
    }

    

    if (description.empty()) {
        description = "Векторный калькулятор выполняющий действие нахождения произведения векторов\n"
                      "Инструкция: -f База данных пользователей -p Порт -e Файл ошибок\n"
                      "-h Помощь\n"
                      "-f Название файла\n"
                      "-p Порт\n"
                      "-e Файл ошибок\n";
    }

    return true;
}
