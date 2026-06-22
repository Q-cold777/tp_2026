#include "Delimiters.hpp"
// Проверяем, совпадает ли следующий символ в потоке с ожидаемым
std::istream& operator>>(std::istream& in, DelimiterChar&& exp)
{
    std::istream::sentry guard(in); // Безопасность: проверяем, готов ли поток к чтению
    if (!guard)
    {
        return in;
    }
    char c = 0;
    in >> c; // Считываем символ из потока (пропускаются пробелы автоматически)
    if (c != exp.expected)
    {
        // Если символ не тот, что мы ждали, заставляем поток выкинуть ошибку
        in.setstate(std::ios_base::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, DelimiterString&& exp)
{
    std::istream::sentry guard(in);
    if (!guard)
    {
        return in;
    }
    for (char expected_char : exp.expected)
    {
        char c = 0;
        in >> c;
        if (c != expected_char)
        {
            in.setstate(std::ios_base::failbit); // Ошибка, если хоть один символ не совпал
            break;
        }
    }
    return in;
}
