#ifndef DELIMITERS_HPP
#define DELIMITERS_HPP

#include <string>
#include <iostream>

struct DelimiterChar
{
    char expected; // Структура для проверки одиночного символа-разделителя
};

struct DelimiterString
{
    std::string expected;
};

std::istream& operator>>(std::istream& in, DelimiterChar&& exp);
std::istream& operator>>(std::istream& in, DelimiterString&& exp);

#endif // DELIMITERS_HPP
