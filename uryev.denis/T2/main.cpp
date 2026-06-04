#include "T2.hpp"
#include <iostream>

int main() {
    std::vector<DataStruct> data;

    // 1. Чтение с помощью итераторов и std::copy
    std::copy(
        std::istream_iterator<DataStruct>(std::cin),
        std::istream_iterator<DataStruct>(),
        std::back_inserter(data)
    );

    // 2. Сортировка по правилам (a, b, c)
    std::sort(data.begin(), data.end(), [](const DataStruct& a, const DataStruct& b) {
        if (a.key1 != b.key1) return a.key1 < b.key1;
        if (a.key2 != b.key2) return a.key2 < b.key2;
        return a.key3.length() < b.key3.length();
        });

    // 3. Вывод результатов через std::copy и итератор потока
    std::copy(
        data.begin(),
        data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );

    return 0;
}