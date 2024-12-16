/** @file
 * @author Синельникова Т.А.
 * @version 1.0
 * @date 05.12.2024
 * @copyright ИБСТ ПГУ
 * @brief Основной заголовочный файл для сервера.
 *
 * Этот файл содержит определения классов и функций, необходимых для работы сервера,
 * включая обработку ошибок, авторизацию, вычисления и интерфейс командной строки.
 */
#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <getopt.h>
#include <cstdint>
#include <filesystem>
#include <limits>


/**
 * @class Error
 * @brief Класс для обработки и записи ошибок в файл.
 * 
 * Этот класс предоставляет методы для записи ошибок в файл и проверки доступности файлов.
 */
class Error {
public:
    Error();///<-Конструктор по умолчанию
    /**
     * @brief Записывает ошибку в файл.
     *
     * Эта статическая функция записывает переданную строку ошибки в указанный файл.
  	 * Функция также добавляет текущую дату и время к записи ошибки.
  	 * @param error Текст ошибки.
     * @param name Имя файла для записи ошибки.
     */
    static void errors(std::string error, std::string name);
    /**
 * @brief Проверяет доступность файла и записывает ошибку, если файл недоступен.
 * 
 * Эта статическая функция проверяет, доступен ли указанный файл для чтения.
 * Если файл недоступен, функция записывает сообщение об ошибке в указанный файл ошибок.
 * 
 * @param fileName Имя файла для проверки.
 * @param file_error Имя файла для записи ошибок.
 * @return 1, если файл доступен, иначе выбрасывает исключение.
 * @throw RuntimeError Если файл недоступен.
 */
    static int er(std::string fileName, std::string file_error);
};
/**
 * @class Server
 * @brief Класс для управления сервером и обработки подключений клиентов.
 * 
 * Этот класс отвечает за настройку сервера, прослушивание порта и обработку подключений клиентов.
 */
class Server {
public:
/**
 * @brief Конструктор класса Server.
 * 
 * Этот конструктор инициализирует объект класса Server, принимая ссылку на объект класса Error.
 * Объект класса Error используется для обработки и записи ошибок, возникающих в процессе работы сервера.
 * 
 * @param err Ссылка на объект класса Error для обработки ошибок.
 */
    Server(Error& err) : errors(err) {}
    /**
 * @brief Настраивает сервер для прослушивания заданного порта.
 * 
 * Эта функция выполняет настройку сервера для прослушивания подключений на указанном порту.
 * Она создает сокет, устанавливает параметры сокета, настраивает таймауты и привязывает сокет к указанному порту.
 * Если настройка прошла успешно, функция возвращает дескриптор сокета. В случае ошибки возвращается -1, и ошибка записывается в указанный файл.
 * 
 * @param error Ссылка на строку для записи ошибок.
 * @param file_error Имя файла для записи ошибок.
 * @param port Порт, на котором сервер будет прослушивать подключения.
 * @return Дескриптор сокета, если настройка прошла успешно, иначе -1.
 * @throw RuntimeError Если произошла ошибка при настройке сервера.
 */
    int self_addr(std::string& error, std::string& file_error, int port);
    /**
 * @brief Ожидает и принимает подключение клиента.
 * 
 * Эта функция ожидает подключения клиента на указанном дескрипторе сокета.
 * Когда клиент подключается, функция возвращает дескриптор сокета для работы с клиентом.
 * Если подключение не удается, функция возвращает -1 и записывает ошибку в указанный файл.
 * 
 * @param s Дескриптор сокета, на котором сервер прослушивает подключения.
 * @param error Ссылка на строку для записи ошибок.
 * @param file_error Имя файла для записи ошибок.
 * @return Дескриптор сокета для работы с клиентом, если подключение успешно, иначе -1.
 * @throw RuntimeError Если произошла ошибка при подключении клиента.
 */
    int client_addr(int s, std::string& error, std::string& file_error);

private:
    Error& errors;///<-Ссылка на объект класса Error для обработки ошибок.
};
/**
 * @class Authorized
 * @brief Класс для обработки авторизации клиентов.
 * 
 * Этот класс отвечает за проверку авторизационных данных клиента, включая генерацию соли и хеширование паролей.
 */
class Authorized {
public:
/**
 * @brief Конструктор класса Authorized.
 * 
 * Этот конструктор инициализирует объект класса Authorized, принимая ссылку на объект класса Error.
 * Объект класса Error используется для обработки и записи ошибок, возникающих в процессе авторизации клиентов.
 * 
 * @param err Ссылка на объект класса Error для обработки ошибок.
 */
    Authorized(Error& err) : e_error(err) {}
    /**
 * @brief Отправляет сообщение клиенту.
 * 
 * Эта функция отправляет строковое сообщение клиенту через указанный дескриптор сокета.
 * Если при отправке сообщения возникает ошибка, функция записывает сообщение об ошибке и выбрасывает исключение.
 * 
 * @param work_sock Дескриптор сокета для работы с клиентом.
 * @param mess Сообщение для отправки.
  * @throw RuntimeError Если произошла ошибка при отправке сообщения.
 */
    void msgsend(int work_sock, const std::string& mess);
    /**
 * @brief Вычисляет SHA-224 хеш строки.
 * 
 * Эта функция вычисляет SHA-224 хеш для переданной строки и возвращает результат в виде шестнадцатеричной строки.
 * Если при вычислении хеша возникает ошибка, функция записывает сообщение об ошибке и возвращает пустую строку.
 * 
 * @param sah Строка для хеширования.
 * @return Хеш строки в виде шестнадцатеричной строки.
  * @throw RuntimeError Если произошла ошибка при вычислении хеша.
 */
    std::string SHA(const std::string& sah);
    /**
 * @brief Генерирует случайную соль заданной длины.
 * 
 * Эта функция генерирует случайную строку заданной длины, состоящую из символов, выбранных из заданного набора символов.
 * Если при генерации строки возникает ошибка, функция записывает сообщение об ошибке и возвращает пустую строку.
 * 
 * @param length Длина генерируемой строки.
 * @return Сгенерированная соль.
 * @throw RuntimeError Если произошла ошибка при генерации соли.
 */
    std::string salt_generator(const std::size_t length);
    /**
 * @brief Проверяет авторизационные данные клиента.
 * 
 * Эта функция проверяет авторизационные данные клиента, сравнивая их с данными, хранящимися в указанном файле.
 * Если авторизация проходит успешно, функция возвращает 1. В случае ошибки возвращается -1, и ошибка записывается в указанный файл.
 * 
 * @param work_sock Дескриптор сокета для работы с клиентом.
 * @param fileName Имя файла с авторизационными данными.
 * @param file_error Имя файла для записи ошибок.
 * @return 1, если авторизация прошла успешно, иначе -1.
 * @throw RuntimeError Если произошла ошибка при проверке авторизации.
 */
    int authorized(int work_sock, std::string fileName, std::string file_error);

private:
    Error& e_error;///<-Ссылка на объект класса Error для обработки ошибок.
};
/**
 * @class Calculator
 * @brief Класс для выполнения вычислений над векторами.
 * 
 * Этот класс вычисления произведение элементов вектора.
 */
class Calculator {
public:
/**
 * @brief Вычисляет произведение элементов вектора.
 * 
 * Эта статическая функция вычисляет произведение всех элементов в переданном векторе значений типа double.
 * Если вектор пуст, функция возвращает 1.0, так как произведение элементов пустого вектора считается равным 1.
 * 
 * @param vectorValues Вектор значений для вычисления произведения.
 * @return Произведение элементов вектора.
 */

    static double processVector(const std::vector<double>& vectorValues);
};
/**
 * @class RuntimeError
 * @brief Класс для обработки исключений.
 * 
 * Этот класс наследуется от std::invalid_argument и используется для выбрасывания исключений с сообщениями об ошибках.
 */
class RuntimeError : public std::invalid_argument {
public:
/**
 * @brief Конструктор класса RuntimeError.
 * 
 * Этот конструктор инициализирует объект класса RuntimeError, принимая строку с сообщением об ошибке.
 * Объект класса RuntimeError наследуется от std::invalid_argument, что позволяет использовать его как исключение.
 * 
 * @param const const std::string& what_arg Сообщение об ошибке.
 */
    explicit RuntimeError(const std::string& what_arg) : std::invalid_argument(what_arg) {}
    /**
 * @brief Конструктор класса RuntimeError.
 * 
 * Этот конструктор инициализирует объект класса RuntimeError, принимая строку с сообщением об ошибке.
 * Объект класса RuntimeError наследуется от std::invalid_argument, что позволяет использовать его как исключение.
 * 
 * @param const const char* what_arg Сообщение об ошибке.
 */
    explicit RuntimeError(const char* what_arg) : std::invalid_argument(what_arg) {}
};
/**
 * @class Interface
 * @brief Класс для обработки интерфейса командной строки.
 * 
 * Этот класс отвечает за разбор аргументов командной строки и предоставление доступа к параметрам.
 */
class Interface {
public:
/**
 * @brief Конструктор класса Interface.
 * 
 * Этот конструктор инициализирует объект класса Interface, устанавливая значения по умолчанию для его членов.
 * Конструктор использует список инициализации для установки начальных значений для переменных fileName, port, file_error и description.
 * 
 * Инициализирует параметры по умолчанию:
 * - fileName: "base.txt"
 * - port: 33333
 * - file_error: "error.txt"
 * - description: "" (пустая строка)
 */
    Interface() : fileName("base.txt"), port(33333), file_error("error.txt"), description("") {};
    /**
 * @brief Возвращает имя файла с базой данных пользователей.
 * 
 * Эта функция возвращает значение fileName, которое содержит имя файла с базой данных пользователей.
 * Функция является константной, что означает, что она не изменяет состояние объекта.
 * 
 * @return Имя файла с базой данных пользователей.
 */
    std::string getFileName() const;
    /**
 * @brief Возвращает номер порта для сервера.
 * 
 * Эта функция возвращает значение port, которое содержит номер порта, на котором сервер будет прослушивать подключения.
 * Функция является константной, что означает, что она не изменяет состояние объекта.
 * 
 * @return Номер порта для сервера.
 */
    int getPort() const;
    /**
 * @brief Возвращает имя файла для записи ошибок.
 * 
 * Эта функция возвращает значение file_error, которое содержит имя файла, в который будут записываться ошибки.
 * Функция является константной, что означает, что она не изменяет состояние объекта.
 * 
 * @return Имя файла для записи ошибок.
 */
    std::string getErrorFile() const;
    /**
 * @brief Возвращает описание команд и параметров.
 * 
 * Эта функция возвращает значение description, которое содержит описание команд и параметров, используемых в интерфейсе командной строки.
 * Функция является константной, что означает, что она не изменяет состояние объекта.
 * 
 * @return Описание команд и параметров.
 */
    std::string getDescription() const;
    /**
 * @brief Разбирает аргументы командной строки.
 * 
 * Эта функция разбирает аргументы командной строки, переданные при запуске программы.
 * Она обрабатывает различные опции, такие как `--help`, `--file`, `--port`, и `--error`, и устанавливает соответствующие значения.
 * Если разбор аргументов прошел успешно, функция возвращает true. В случае ошибки выбрасывается исключение.
 * 
 * @param argc Количество аргументов.
 * @param argv Массив аргументов.
 * @return true, если разбор прошел успешно, иначе выбрасывает исключение.
 * @throw RuntimeError Если произошла ошибка при разборе аргументов.
 */
	bool parseArguments(int argc, char* argv[]);
private:
    std::string fileName;///<-Имя файла с базой данных пользователей.
    int port;///<-Номер порта
    std::string file_error;///<-Имя файла для записи ошибок.
    std::string description;///<-Описание команд и параметров.

    
};
#endif // SERVER_H
