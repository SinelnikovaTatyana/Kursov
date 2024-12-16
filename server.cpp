/** @file server.cpp
 * @author Синельникова Т.А.
 * @version 1.0
 * @date 05.12.2024
 * @copyright ИБСТ ПГУ
 * @brief Реализация функций для работы сервера.
 *
 * Этот файл содержит реализацию функций, связанных с настройкой и запуском сервера, 
 * обработкой подключений клиентов.
 */
#include "server.h"
#include <iostream>
#include <cassert>
#include <arpa/inet.h>
#include <cstdlib>
#include <unistd.h>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
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


Error::Error() {}

void alarm_handler(int signal) {
    (void)signal; // Помечаем параметр как неиспользуемый
    std::cout << "Время ожидания истекло\n";
    exit(EXIT_FAILURE);
}

int Server::self_addr(std::string& error, std::string& file_error, int port) {
    try {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            throw RuntimeError("Ошибка при создании сокета");
        }

        int on = 1;
        int rc = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if (rc == -1) {
            throw RuntimeError("Ошибка установки сокета");
        }

        signal(SIGALRM, alarm_handler);
        alarm(240);

        struct timeval timeout {240, 0};
        rc = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        if (rc == -1) {
            throw RuntimeError("Ошибка установки сокета");
        }

        sockaddr_in self_addr;
        self_addr.sin_family = AF_INET;
        self_addr.sin_port = htons(port);
        self_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        std::cout << "Сервер запущен на порту: " << port << std::endl; 

        std::cout << "Ожидание подключения клиента...\n";

        int b = bind(sock, reinterpret_cast<sockaddr*>(&self_addr), sizeof(self_addr));
        if (b == -1) {
            throw RuntimeError("Ошибка привязки");
        }

        listen(sock, SOMAXCONN);

        return sock;
    } catch (const RuntimeError& e) {
        std::cerr << e.what() << std::endl;
        error = "Ошибка";
        errors.errors(error, file_error);
        return -1;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return -1;
    }
}

int Server::client_addr(int s, std::string& error, std::string& file_error) {
    try {
        sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);

        int work_sock = accept(s, reinterpret_cast<sockaddr*>(&client_addr), &len);
        if (work_sock == -1) {
            throw RuntimeError("Ошибка при подключении клиента");
        }

        std::cout << "Клиент успешно подключился!\n";
        return work_sock;
    } catch (const RuntimeError& e) {
        std::cerr << e.what() << std::endl;
        error = "Ошибка";
        errors.errors(error, file_error);
        return -1;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return -1;
    }
}
