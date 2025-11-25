#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <clocale>

std::mutex m;
int coins = 101;

void coin_sharing(std::string name, int& thief_coins, int& companion_coins) {
    while (true) {
        m.lock();

        if (coins == 0) {
            m.unlock();
            break;
        }

        if (coins == 1 && thief_coins == companion_coins) {
            m.unlock();
            break;
        }

        if (thief_coins <= companion_coins) {
            coins--;
            thief_coins++;
            std::cout << name << " : " << thief_coins << " " << companion_coins
                << " (Осталось: " << coins << ")" << std::endl;
            m.unlock();
        }
        else {
            m.unlock();
            std::this_thread::yield();
        }
    }
}

int main() {
    setlocale(LC_ALL, "RU");

    int Bob_coins = 0;
    int Tom_coins = 0;

    std::cout << "Начинаем дележ " << coins << " монет..." << std::endl;

    std::thread t1(coin_sharing, "Bob", std::ref(Bob_coins), std::ref(Tom_coins));
    std::thread t2(coin_sharing, "Tom", std::ref(Tom_coins), std::ref(Bob_coins));

    t1.join();
    t2.join();

    std::cout << "\n--- Итог ---" << std::endl;
    std::cout << "Bob: " << Bob_coins << std::endl;
    std::cout << "Tom: " << Tom_coins << std::endl;
    std::cout << "Осталось монет (покойнику): " << coins << std::endl;
    std::cout << "Сумма (Bob + Tom + Остаток): " << (Bob_coins + Tom_coins + coins) << std::endl;

    system("pause");
    return 0;
}