#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include <string>
#include <algorithm>
#include <clocale>

class CustomMutex {
private:
    std::atomic<bool> locked;
public:
    CustomMutex() {
        locked.store(false);
    }

    void lock() {
        bool expected = false;
        while (!locked.compare_exchange_strong(expected, true)) {
            expected = false;
            std::this_thread::yield();
        }
    }

    void unlock() {
        locked.store(false);
    }
};

CustomMutex m;
int dishes[3];
int eaten[3];
bool exploded[3];
std::atomic<bool> game_over;
std::string result_message;

void cook_work(int efficiency) {
    auto start_time = std::chrono::steady_clock::now();

    while (!game_over) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        auto current_time = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count() >= 5) {
            m.lock();
            if (!game_over) {
                result_message = "3) Кук уволился сам! (Прошло 5 дней)";
                game_over = true;
            }
            m.unlock();
            return;
        }

        m.lock();
        if (game_over) {
            m.unlock();
            return;
        }
        for (int i = 0; i < 3; ++i) {
            dishes[i] += efficiency;
        }
        m.unlock();

        std::this_thread::yield();
    }
}

void fat_man_eat(int id, int gluttony) {
    while (!game_over) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        m.lock();

        if (game_over) {
            m.unlock();
            return;
        }

        if (exploded[id]) {
            m.unlock();
            std::this_thread::yield();
            continue;
        }

        if (dishes[id] == 0) {
            result_message = "1) Кука уволили! (Тарелка пуста)";
            game_over = true;
            m.unlock();
            return;
        }

        int space_left = 10000 - eaten[id];
        int bite = std::min(gluttony, space_left);

        if (dishes[id] < bite) {
            bite = dishes[id];
        }

        dishes[id] -= bite;
        eaten[id] += bite;

        if (eaten[id] >= 10000) {
            exploded[id] = true;
        }

        if (exploded[0] && exploded[1] && exploded[2]) {
            result_message = "2) Кук не получил зарплату. (Все лопнули)";
            game_over = true;
            m.unlock();
            return;
        }

        m.unlock();
        std::this_thread::yield();
    }
}

void run_simulation(int gluttony, int efficiency, std::string test_name) {
    game_over = false;
    result_message = "";
    for (int i = 0; i < 3; ++i) {
        dishes[i] = 3000;
        eaten[i] = 0;
        exploded[i] = false;
    }

    std::cout << "\n=== Тест: " << test_name << " ===" << std::endl;
    std::cout << "Параметры: gluttony=" << gluttony << ", efficiency=" << efficiency << std::endl;
    std::cout << "Симуляция..." << std::endl;

    std::thread cook(cook_work, efficiency);
    std::thread t1(fat_man_eat, 0, gluttony);
    std::thread t2(fat_man_eat, 1, gluttony);
    std::thread t3(fat_man_eat, 2, gluttony);

    cook.join();
    t1.join();
    t2.join();
    t3.join();

    std::cout << "Результат: " << result_message << std::endl;
    std::cout << "На тарелках: " << dishes[0] << ", " << dishes[1] << ", " << dishes[2] << std::endl;
    std::cout << "Съедено: " << eaten[0] << ", " << eaten[1] << ", " << eaten[2] << std::endl;
}

int main() {
    setlocale(LC_ALL, "RU");

    run_simulation(100, 10, "Повар не успевает");

    run_simulation(60, 200, "Толстяки лопаются");

    run_simulation(1, 10, "Идеальный баланс (5 сек)");

    std::cout << "\nВсе тесты завершены." << std::endl;
    system("pause");
    return 0;
}