#include "DataStruct.h"
#include <cstdlib>
#include <algorithm>
#include <cctype>

// ============ КОНСТРУКТОРЫ ============
DataStruct::DataStruct() : key1(0), key2('\0'), key3("") {}

DataStruct::DataStruct(unsigned long long k1, char k2, const std::string& k3)
    : key1(k1), key2(k2), key3(k3) {}

DataStruct::DataStruct(const DataStruct& other)
    : key1(other.key1), key2(other.key2), key3(other.key3) {}

DataStruct::DataStruct(DataStruct&& other) noexcept
    : key1(std::move(other.key1)), key2(std::move(other.key2)),
      key3(std::move(other.key3)) {}

// ============ ОПЕРАТОРЫ ПРИСВАИВАНИЯ ============
DataStruct& DataStruct::operator=(const DataStruct& other) {
    if (this != &other) {
        DataStruct temp(other);
        swap(temp);
    }
    return *this;
}

DataStruct& DataStruct::operator=(DataStruct&& other) noexcept {
    if (this != &other) {
        swap(other);
    }
    return *this;
}

void DataStruct::swap(DataStruct& other) noexcept {
    using std::swap;
    swap(key1, other.key1);
    swap(key2, other.key2);
    swap(key3, other.key3);
}

// ============ ПАРСЕР KEY1 (ULL HEX) ============
bool DataStruct::parseKey1(const std::string& valueStr,
                           unsigned long long& result) {
    std::string trimmed = valueStr;
    size_t start = trimmed.find_first_not_of(" \t");
    size_t end = trimmed.find_last_not_of(" \t");
    if (start == std::string::npos) return false;
    trimmed = trimmed.substr(start, end - start + 1);

    if (trimmed.length() < 3 || trimmed[0] != '0') return false;
    if (trimmed[1] != 'x' && trimmed[1] != 'X') return false;

    char* endPtr;
    unsigned long long value = std::strtoull(trimmed.c_str(), &endPtr, 16);
    if (*endPtr != '\0') return false;

    result = value;
    return true;
}

// ============ ПАРСЕР KEY2 (CHR LIT) ============
bool DataStruct::parseKey2(const std::string& valueStr, char& result) {
    std::string trimmed = valueStr;
    size_t start = trimmed.find_first_not_of(" \t");
    size_t end = trimmed.find_last_not_of(" \t");
    if (start == std::string::npos) return false;
    trimmed = trimmed.substr(start, end - start + 1);

    if (trimmed.length() != 3) return false;
    if (trimmed[0] != '\'' || trimmed[2] != '\'') return false;

    result = trimmed[1];
    return true;
}

// ============ ПАРСЕР KEY3 (STRING) ============
bool DataStruct::parseKey3(const std::string& valueStr, std::string& result) {
    std::string trimmed = valueStr;
    size_t start = trimmed.find_first_not_of(" \t");
    if (start == std::string::npos) return false;
    trimmed = trimmed.substr(start);

    if (trimmed[0] != '"') return false;

    size_t end = 1;
    while (end < trimmed.length()) {
        if (trimmed[end] == '"') {
            break;
        }
        end++;
    }
    if (end >= trimmed.length()) return false;

    result = trimmed.substr(1, end - 1);
    return true;
}

// ============ ОПЕРАТОР ВВОДА ============
std::istream& operator>>(std::istream& is, DataStruct& data) {
    std::string line;

    while (std::getline(is, line)) {
        // Ищем открывающую скобку
        size_t openParen = line.find('(');
        if (openParen == std::string::npos) continue;

        // Ищем закрывающую скобку
        size_t closeParen = line.rfind(')');
        if (closeParen == std::string::npos ||
            closeParen <= openParen) continue;

        // Извлекаем содержимое между скобками
        std::string content = line.substr(openParen + 1,
                                          closeParen - openParen - 1);

        unsigned long long k1 = 0;
        char k2 = '\0';
        std::string k3;
        bool foundKey1 = false, foundKey2 = false, foundKey3 = false;

        size_t pos = 0;
        while (pos < content.length()) {
            // Ищем двоеточие (начало поля)
            size_t colonPos = content.find(':', pos);
            if (colonPos == std::string::npos) break;

            // Ищем пробел после имени поля
            size_t spacePos = content.find(' ', colonPos + 1);
            if (spacePos == std::string::npos) break;

            // Имя поля
            std::string fieldName = content.substr(colonPos + 1,
                                                   spacePos - colonPos - 1);

            // Ищем следующее двоеточие (конец значения)
            size_t nextColon = content.find(':', spacePos + 1);
            std::string fieldValue;
            if (nextColon == std::string::npos) {
                fieldValue = content.substr(spacePos + 1);
            } else {
                fieldValue = content.substr(spacePos + 1,
                                           nextColon - spacePos - 1);
            }
            pos = (nextColon == std::string::npos) ?
                  content.length() : nextColon;

            // Парсим поле
            if (fieldName == "key1") {
                foundKey1 = DataStruct::parseKey1(fieldValue, k1);
            } else if (fieldName == "key2") {
                foundKey2 = DataStruct::parseKey2(fieldValue, k2);
            } else if (fieldName == "key3") {
                foundKey3 = DataStruct::parseKey3(fieldValue, k3);
            }
        }

        // Если нашли все три поля — заполняем структуру и возвращаем
        if (foundKey1 && foundKey2 && foundKey3) {
            data.key1 = k1;
            data.key2 = k2;
            data.key3 = k3;
            return is;
        }
        // Иначе продолжаем цикл — ищем следующую строку
    }

    // Если дошли сюда — поток закончился или нет правильных строк
    is.setstate(std::ios::failbit);
    return is;
}

// ============ ОПЕРАТОР ВЫВОДА ============
std::ostream& operator<<(std::ostream& os, const DataStruct& data) {
    os << "(:key1 0x" << std::hex << std::uppercase << data.key1
       << std::dec << std::nouppercase;
    os << ":key2 '" << data.key2 << "'";
    os << ":key3 \"" << data.key3 << "\":)";
    return os;
}

// ============ КОМПАРАТОР ДЛЯ СОРТИРОВКИ ============
bool DataStructComparator::operator()(const DataStruct& lhs,
                                      const DataStruct& rhs) const {
    if (lhs.key1 != rhs.key1) {
        return lhs.key1 < rhs.key1;
    }
    if (lhs.key2 != rhs.key2) {
        return lhs.key2 < rhs.key2;
    }
    return lhs.key3.length() < rhs.key3.length();
}
