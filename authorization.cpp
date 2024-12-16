/** @file authorization.cpp
 * @author Синельникова Т.А.
 * @version 1.0
 * @date 05.12.2024
 * @copyright ИБСТ ПГУ
 * @brief Реализация функций для обработки авторизации клиентов.
 *
 * Этот файл содержит реализацию функций, связанных с авторизацией клиентов,
 * включая отправку сообщений, вычисление хешей, генерацию соли и проверку авторизации.
 */
#include "server.h"

void Authorized::msgsend(int work_sock, const std::string& mess) {
    try {
        char *buffer = new char[4096];
        try {
            std::strcpy(buffer, mess.c_str());
        } catch (const std::exception& e) {
            delete[] buffer;
            throw RuntimeError("Ошибка копирования строки: " + std::string(e.what()));
        }

        try {
            send(work_sock, buffer, mess.length(), 0);
        } catch (const std::exception& e) {
            delete[] buffer;
            throw RuntimeError("Ошибка отправки данных: " + std::string(e.what()));
        }

        delete[] buffer;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка отправки сообщения: " << e.what() << std::endl;
    }
}

std::string Authorized::SHA(const std::string& sah) {
    try {
        CryptoPP::SHA224 hash;
        std::string digest;
        CryptoPP::StringSource(sah, true,
            new CryptoPP::HashFilter(hash,
                new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest))));
        return digest;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка хеширования: " << e.what() << std::endl;
        return "";
    }
}

std::string Authorized::salt_generator(const std::size_t length) {
    try {
        const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::string salt;
        std::random_device rd;
        std::default_random_engine rng(rd());
        std::uniform_int_distribution<int> dist(0, sizeof(charset) - 2);

        for (std::size_t i = 0; i < length; i++) {
            salt += charset[dist(rng)];
        }

        return salt;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка генерации соли: " << e.what() << std::endl;
        return "";
    }
}

int Authorized::authorized(int work_sock, std::string fileName, std::string file_error) {
    try {
        std::string ok = "OK";
        std::string err = "ERR";
        std::string error;
        char msg[255];

        std::string login, hashq;
        std::fstream file;
        file.open(fileName);
        if (!file.is_open()) {
            throw RuntimeError("Failed to open file: " + fileName);
        }

        getline(file, login, ':');
        getline(file, hashq);
        std::cout << "Read login from file: " << login << std::endl;
        std::cout << "Read hash from file: " << hashq << std::endl;

        // Получаем сообщение от клиента
        int bytes_received = recv(work_sock, msg, sizeof(msg) - 1, 0);
        if (bytes_received <= 0) {
            throw RuntimeError("Ошибка при получении данных от клиента");
        }
        msg[bytes_received] = '\0'; // завершающий нулевой символ
        std::string message = msg;
        int mess=message.size();
        std::cout << "Received message from client: " << message << "    len " << message.size() << std::endl;
        
		std::string client_hash = message.substr(mess-56,56);
        std::string client_login = message.substr(0, mess-72);
        std::string client_salt = message.substr(mess-72,16);
        std::cout << "Extracted salt from client: " << client_salt << std::endl;
        std::cout << "Extracted login from client: " << client_login << std::endl;
        std::cout << "Extracted hash from client: " << client_hash << std::endl;

        if (client_login != login) {
            std::cout << "Login mismatch: " << client_login << " != " << login << std::endl;
            msgsend(work_sock, err);
            
            close(work_sock);
            return -1;
        } else {
            std::string sah = client_salt + hashq;
            std::cout << "password " << hashq << std::endl;
            std::string server_hash = SHA(sah);
            std::cout << "Server hash: " << server_hash << std::endl;

            if (server_hash != client_hash) {
                std::cout << "Hash mismatch: " << server_hash << " != " << client_hash << std::endl;
                msgsend(work_sock, err);
               
                close(work_sock);
                return -1;
            } else {
                std::cout << "Authentication successful" << std::endl;
                msgsend(work_sock, ok);
            }
        }
        return 1;
    } catch (const RuntimeError& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return -1;
    }
}
