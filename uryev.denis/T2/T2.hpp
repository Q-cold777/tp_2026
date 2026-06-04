#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <cmath>

struct DataStruct {
    double key1;
    unsigned long long key2;
    std::string key3;
};

struct Delimiter {
    char expected;
};

std::istream& operator>>(std::istream& in, Delimiter&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    char c;
    in >> c;
    if (std::tolower(c) != std::tolower(dest.expected)) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

// Перегрузка оператора ввода согласно заданию
std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    DataStruct temp;
    // Флаги для проверки, что все 3 ключа были считаны
    bool has_key1 = false;
    bool has_key2 = false;
    bool has_key3 = false;

    // Формат начала: (:
    if (!(in >> Delimiter{ '(' } >> Delimiter{ ':' })) return in;

    for (int i = 0; i < 3; ++i) {
        std::string label;
        in >> label;

        if (label == "key1") {
            // Чтение DBL SCI (научный формат)
            if (!(in >> std::scientific >> temp.key1)) return in;
            has_key1 = true;
        }
        else if (label == "key2") {
            // Чтение ULL HEX (0x...)
            if (!(in >> std::hex >> temp.key2)) return in;
            has_key2 = true;
        }
        else if (label == "key3") {
            // Чтение строки в кавычках
            if (!(in >> Delimiter{ '"' })) return in;
            std::getline(in, temp.key3, '"');
            has_key3 = true;
        }
        else {
            in.setstate(std::ios::failbit);
            return in;
        }

        // После каждого ключа идет двоеточие
        if (!(in >> Delimiter{ ':' })) return in;
    }

    // Закрывающая скобка: )
    if (!(in >> Delimiter{ ')' })) return in;

    // Если все ключи успешно считаны, обновляем dest
    if (in && has_key1 && has_key2 && has_key3) {
        dest = temp;
    }
    else {
        in.setstate(std::ios::failbit);
    }
    return in;
}

// Перегрузка оператора вывода согласно заданию
std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    std::ostream::sentry sentry(out);
    if (!sentry) return out;

    std::ios_base::fmtflags f(out.flags());
    out << "(:key1 ";

    // Вывод DBL SCI: мантисса [1, 10), точность до десятых, нижний регистр
    out << std::scientific << std::setprecision(1) << std::nouppercase << src.key1;

    // Вывод ULL HEX: 0x, верхний регистр цифр
    out << ":key2 0x" << std::hex << std::uppercase << src.key2;

    // Вывод строки
    out << ":key3 \"" << src.key3 << "\":)";

    out.flags(f);
    return out;
}