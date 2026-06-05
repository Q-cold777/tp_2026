#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <iostream>
#include <string>

struct DataStruct {
    // Поля структуры
    unsigned long long key1;      // ULL HEX //  целое число (шестнадцатеричное число)
    char key2;                     // CHR LIT // символ (символ в кавычках)
    std::string key3;             // строка в кавычках

    // Конструкторы
    DataStruct();
    DataStruct(unsigned long long k1, char k2, const std::string& k3);
    DataStruct(const DataStruct& other);
    DataStruct(DataStruct&& other) noexcept;

    // Операторы присваивания
    DataStruct& operator=(const DataStruct& other);
    DataStruct& operator=(DataStruct&& other) noexcept;

    // Операторы ввода/вывода
    friend std::istream& operator>>(std::istream& is, DataStruct& data); //Читает строку, парсит, заполняет поля
    friend std::ostream& operator<<(std::ostream& os, const DataStruct& data); //Выводит поля в форматированном виде

    void swap(DataStruct& other) noexcept;

//методы-парсеры
//Каждый из них берёт строку, пытается превратить её в нужный тип и сохраняет в result.
private:
    static bool parseKey1(const std::string& valueStr, unsigned long long& result); //Шестнадцатеричное число
    static bool parseKey2(const std::string& valueStr, char& result); //Символ в кавычках
    static bool parseKey3(const std::string& valueStr, std::string& result); //Строку в кавычках
};

struct DataStructComparator {
    bool operator()(const DataStruct& lhs, const DataStruct& rhs) const;
};

#endif
