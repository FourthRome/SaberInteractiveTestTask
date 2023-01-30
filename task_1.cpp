#include <iostream>
#include <limits.h>
#include <string>

const size_t BitsInInt = sizeof(int) * 8;

std::string ToBinary(int n) {
    unsigned n_raw = n;
    std:: string result(BitsInInt, '0');
    int idx = result.size() - 1;
    while (n_raw > 0) {
        if (n_raw % 2 == 1) {
            result[idx] = '1';
        }
        n_raw >>= 1;
        idx -= 1;
    }
    return result;
}

std::string ToBinaryNoTrailing(int n) {
    unsigned n_raw = n;
    size_t highest_power = BitsInInt - 1;
    unsigned mask = 0x1 << highest_power;
    while ((n_raw & mask) == 0 && (highest_power > 0)) {
        highest_power -= 1;
        mask >>= 1;
    }
    std::string result(highest_power + 1, '0');
    int idx = result.size() - 1;
    while (n_raw > 0) {
        if (n_raw % 2 == 1) {
            result[idx] = '1';
        }
        n_raw >>= 1;
        idx -= 1;
    }
    return result;
}

int main() {
    std::cout << ToBinaryNoTrailing(1) << std::endl;
    std::cout << ToBinaryNoTrailing(10) << std::endl;
    std::cout << ToBinaryNoTrailing(-1) << std::endl;
    std::cout << ToBinaryNoTrailing(INT_MAX) << std::endl;
    std::cout << ToBinaryNoTrailing(INT_MIN) << std::endl;
    return 0;
}
