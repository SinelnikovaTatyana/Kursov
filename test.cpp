#include <UnitTest++/UnitTest++.h>
#include <UnitTest++/TestReporterStdout.h>
#include "server.h"
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <limits>
#include <iostream>

													  // Тестирование справки


SUITE(HelpTest)
{
	TEST(ShortHelp) {
    	Interface iface;
    	const char* args[] = {"test", "-h"};
    	iface.parseArguments(2, const_cast<char**>(args));
    	CHECK(!iface.getDescription().empty());
    }

    TEST(LongHelp) {
    	Interface iface;
    	const char* args[] = {"test", "--help"};
    	iface.parseArguments(2, const_cast<char**>(args));
    	CHECK(!iface.getDescription().empty());
    }
    
    TEST(HelpAndFile) {
    	Interface iface;
    	const char* args[] = {"test", "-h", "-f", "base.txt"};
    	iface.parseArguments(4, const_cast<char**>(args));
    	CHECK(!iface.getDescription().empty());
    }
	
	TEST(FileAndHelp) {
    	Interface iface;
    	const char* args[] = {"test", "-f", "base.txt", "-h"};
    	iface.parseArguments(4, const_cast<char**>(args));
    	CHECK(!iface.getDescription().empty());
    }
	TEST(HelpAndPort) {
    	Interface iface;
    	const char* args[] = {"test", "-h", "-p", "8080"};
    	iface.parseArguments(4, const_cast<char**>(args));
    	CHECK(!iface.getDescription().empty());
    }
	TEST(PortAndHelp) {
    	Interface iface;
    	const char* args[] = {"test", "-p", "8080", "-h" };
    	iface.parseArguments(4, const_cast<char**>(args));
    	CHECK(!iface.getDescription().empty());
    }
    TEST(HelpAndErrorFile) {
    	Interface iface;
    	const char* args[] = {"test", "-h", "-e", "error.txt" };
    	iface.parseArguments(4, const_cast<char**>(args));
    	CHECK(!iface.getDescription().empty());
    }
    TEST(ErrorFileAndHelp) {
    	Interface iface;
    	const char* args[] = {"test", "-e", "error.txt", "-h"};
    	iface.parseArguments(4, const_cast<char**>(args));
    	CHECK(!iface.getDescription().empty());
    }
}

 											// Тестирование рабочих параметров
SUITE(WorkParamsTest) {
  TEST(ShortPort) {
  		optind = 0;
    	Interface iface;
    	const char* args[] = {"test", "-p", "33333"};
    	iface.parseArguments(3, const_cast<char**>(args));
    	CHECK_EQUAL(iface.getPort(), 33333);
    }
  TEST(LongPort) {
  		optind = 0;
    	Interface iface;
    	const char* args[] = {"test", "--port", "33333"};
    	CHECK(iface.parseArguments(3, const_cast<char**>(args)));
    	CHECK_EQUAL(33333,iface.getPort());
    }
  TEST(PortAndDataFileAndLogFile) {
  		optind = 0;
    	Interface iface;
    	const char* args[] = {"test", "-p", "33333", "-f", "base.txt", "-e", "error.txt"};
    	iface.parseArguments(7, const_cast<char**>(args));
    	CHECK_EQUAL(iface.getPort(), 33333);
    	CHECK_EQUAL(iface.getFileName(), "base.txt");
    	CHECK_EQUAL(iface.getErrorFile(), "error.txt");
    }
  TEST(DataFileAndLogFileAndPort) {
  		optind = 0;
    	Interface iface;
    	const char* args[] = {"test", "-f", "base.txt", "-e", "error.txt", "-p", "33333"};
    	iface.parseArguments(7, const_cast<char**>(args));
    	CHECK_EQUAL(iface.getPort(), 33333);
    	CHECK_EQUAL(iface.getFileName(), "base.txt");
    	CHECK_EQUAL(iface.getErrorFile(), "error.txt");
    }
  TEST(Default) {
  		optind = 0;
    	Interface iface;
    	const char* args[] = {"test"};
    	iface.parseArguments(1, const_cast<char**>(args));
    	CHECK_EQUAL(iface.getPort(), 33333);
    	CHECK_EQUAL(iface.getFileName(), "base.txt");
    	CHECK_EQUAL(iface.getErrorFile(), "error.txt");
    }
  TEST(MissingPort) {
  		optind = 0;
    	Interface iface;
    	const char* args[] = {"test", "-f", "base.txt", "-e", "error.txt"};
    	iface.parseArguments(5, const_cast<char**>(args));
    	CHECK_EQUAL(iface.getPort(), 33333);
    }
  TEST(InvalidPort) {
  		optind = 0; 
    	Interface iface;
    	const char* args[] = {"test", "-p", "65537"};
    	CHECK_THROW(iface.parseArguments(3, const_cast<char**>(args)), RuntimeError);
    }
}

												// Тестирование исключений
SUITE(ErrorTest)
{
     
    TEST(InvalidFileName) {
    	optind = 0; 
    	Interface iface;
        const char* args[] = {"test", "-f", ""};
        CHECK_THROW(iface.parseArguments(3, const_cast<char**>(args)), RuntimeError);
    }
    TEST(InvalidName) {
    	optind = 0; 
    	Interface iface;
        const char* args[] = {"test", "-f", "dana.txt"};
        CHECK_THROW(iface.parseArguments(3, const_cast<char**>(args)), RuntimeError);
    }
    TEST(InvalidParametr) {
    	optind = 0; 
    	Interface iface;
        const char* args[] = {"test", "-f", "base.txt","-v"};
        CHECK_THROW(iface.parseArguments(4, const_cast<char**>(args)), RuntimeError);
    }

    TEST(InvalidErrorFile) {
    	optind = 0; 
    	Interface iface;
        const char* args[] = {"test", "-e", ""};
        CHECK_THROW(iface.parseArguments(3, const_cast<char**>(args)), RuntimeError);
    }
    TEST(InvalidErrorName) {
    	optind = 0; 
    	Interface iface;
        const char* args[] = {"test", "-e", "eror.txt"};
        CHECK_THROW(iface.parseArguments(3, const_cast<char**>(args)), RuntimeError);
    }
}
													// Тестирование калькулятора
SUITE(CalculatorTest)
{
    // Тест на нормальный случай
    TEST(ProcessVector_NormalCase) {
        std::vector<double> vector = {-4000.0, -5000.0, -6000.0, -7000.0};
        double expected_product = 8.4e+14; 

        double result = Calculator::processVector(vector);
        CHECK_EQUAL(expected_product, result);
    }

    // Тест на пустой вектор
    TEST(ProcessVector_EmptyVector) {
        std::vector<double> vector;
        double expected_product = 1.0; // Предполагаем, что произведение пустого вектора равно 1

        double result = Calculator::processVector(vector);
        CHECK_EQUAL(expected_product, result);
    }

    // Тест на вектор с одним элементом
    TEST(ProcessVector_SingleElement) {
        std::vector<double> vector = {5.0};
        double expected_product = 5.0;

        double result = Calculator::processVector(vector);
        CHECK_EQUAL(expected_product, result);
    }

    // Тест на вектор с нулевым элементом
    TEST(ProcessVector_ZeroElement) {
        std::vector<double> vector = {2.0, 0.0, 4.0};
        double expected_product = 0.0;

        double result = Calculator::processVector(vector);
        CHECK_EQUAL(expected_product, result);
    }

    // Тест на большой ввод
    TEST(ProcessVector_LargeInput) {
        std::vector<double> vector(1000000, 1.0);
        double expected_product = 1.0; // Произведение всех единиц равно 1

        double result = Calculator::processVector(vector);
        CHECK_EQUAL(expected_product, result);
    }
}

									// Тестирование авторизации
SUITE(AuthTest) {

    TEST(NoUser) {
        std::ofstream file("b.txt");
        file << "user1:password1\n";
        file.close();

        Error errors;
        Authorized auth(errors);

        // Подготовка тестовой строки с несуществующим логином
        std::string input = "user2" + std::string(16, '0') + std::string(56, 'a');

        // Проверка неуспешного входа
        int result = auth.authorized(0, "b.txt", "error.txt"); // 0 - заглушка для сокета
        CHECK_EQUAL(-1, result);

        std::remove("b.txt");
    }

 	TEST(YesUser) {
        std::ofstream file("b.txt");
        file << "user1:password1\n";
        file.close();

        Error errors;
        Authorized auth(errors);

        // Подготовка тестовой строки
        std::string login = "user1";
        std::string salt = "0000000000000000"; // 16 символов
        std::string password = "password1";
        std::string hashedPassword = auth.SHA(salt + password);
        std::string input = login + salt + hashedPassword;

        // Создаем заглушку для сокета
        int sockfd[2];
        socketpair(AF_UNIX, SOCK_STREAM, 0, sockfd);

        // Отправляем данные в сокет
        send(sockfd[1], input.c_str(), input.size(), 0);

        // Проверка успешного входа
        int result = auth.authorized(sockfd[0], "b.txt", "error.txt");
        CHECK_EQUAL(1, result);

        close(sockfd[0]);
        close(sockfd[1]);
        std::remove("b.txt");
    }
    
    TEST(hashPassword) {
        Error errors;
        Authorized auth(errors);

        std::string salt = "3C0C3F8F7F842B10";
        std::string hash = "91ABAB6886D1E71DFBE9B2A15830D0C8B33AFE2F3E1980D599EEAB9C";
        std::string pass = "P@ssW0rd";
        std::string res = auth.SHA(salt + pass);

        CHECK_EQUAL(res, hash);
    }
    
    TEST(InvalidHash) {
        std::ofstream file("b.txt");
        file << "user1:password1\n";
        file.close();

        Error errors;
        Authorized auth(errors);

        // Подготовка тестовой строки с неправильным хэшем
        std::string input = "user1" + std::string(16, '0') + std::string(56, 'a');

        // Создаем заглушку для сокета
        int sockfd[2];
        socketpair(AF_UNIX, SOCK_STREAM, 0, sockfd);

        // Отправляем данные в сокет
        send(sockfd[1], input.c_str(), input.size(), 0);

        // Проверка неуспешного входа
        int result = auth.authorized(sockfd[0], "b.txt", "error.txt");
        CHECK_EQUAL(-1, result);

        close(sockfd[0]);
        close(sockfd[1]);
        std::remove("b.txt");
    }
    
    TEST(InvalidLength) {
        std::ofstream file("b.txt");
        file << "user1:password1\n";
        file.close();

        Error errors;
        Authorized auth(errors);

        // Подготовка некорректной строки
        std::string input = "user1" + std::string(10, '0');

        // Создаем заглушку для сокета
        int sockfd[2];
        socketpair(AF_UNIX, SOCK_STREAM, 0, sockfd);

        // Отправляем данные в сокет
        send(sockfd[1], input.c_str(), input.size(), 0);

        // Проверка неуспешного входа
        int result = auth.authorized(sockfd[0], "b.txt", "error.txt");
        CHECK_EQUAL(-1, result);

        close(sockfd[0]);
        close(sockfd[1]);
        std::remove("b.txt");
    }
    
}

int main() {
    return UnitTest::RunAllTests();
}
