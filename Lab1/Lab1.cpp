#include <iostream>
#include <string>
#include <ctime>
#include <thread>
#include <vector>
#include <mutex>
#include <clocale>

void do_work(std::string name) {
    long long factorial = 1;
    for (int j = 0; j < 10000000; ++j) {
        factorial = 1;
        for (int i = 1; i <= 10; ++i) {
            factorial *= i;
        }
    }
}

int main() {
    setlocale(LC_ALL, "RU");

    std::cout << "Упражнение 1: Замер времени 10!" << std::endl;
    clock_t start = clock();
    do_work("main");
    clock_t end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    std::cout << "Время выполнения: " << seconds << " сек." << std::endl;

    std::cout << "\nУпражнение 2: Сравнение последовательного и параллельного запуска" << std::endl;

    start = clock();
    do_work("t1");
    do_work("t2");
    end = clock();
    seconds = (double)(end - start) / CLOCKS_PER_SEC;
    std::cout << "Последовательный запуск (2 раза): " << seconds << " сек." << std::endl;

    start = clock();
    std::thread t1(do_work, "t1");
    std::thread t2(do_work, "t2");
    t1.join();
    t2.join();
    end = clock();
    seconds = (double)(end - start) / CLOCKS_PER_SEC;
    std::cout << "Параллельный запуск (2 потока): " << seconds << " сек." << std::endl;

    system("pause");
    return 0;
}