#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <limits>
#include <cmath>
#include <sstream>
#include "dataStruct.hpp"

// Вспомогательная функция для сравнения структур (компаратор)
bool compareDataStruct(
    const DataStruct &a,
    const DataStruct &b)
{
  const double EPS = 1e-9;

  double absA1 = std::abs(a.key1);
  double absB1 = std::abs(b.key1);

  // 1. Сравнение по абсолютному значению key1
  if (std::abs(absA1 - absB1) > EPS)
  {
    return absA1 < absB1;
  }

  // 2. Сравнение по key2
  if (std::abs(a.key2 - b.key2) > EPS)
  {
    return a.key2 < b.key2;
  }

  // 3. Сравнение по длине строки key3
  return a.key3.length() < b.key3.length();
}

int main()
{
  std::vector<DataStruct> data;
  std::string line;

  // Построчно читаем весь стандартный ввод
  while (std::getline(std::cin, line))
  {
    if (line.empty())
    {
      continue;
    }

    // Передаем строку в stringstream для безопасного парсинга
    std::stringstream ss(line);
    DataStruct temp;

    if (ss >> temp)
    {
      data.push_back(temp);
    }
    else
    {
      // Если строка невалидна, просто игнорируем ее и идем дальше
      std::cin.clear();
    }
  }

  // Сортируем собранные валидные данные
  std::sort(
      data.begin(),
      data.end(),
      compareDataStruct);

  // Выводим результат в консоль
  std::ostream_iterator<DataStruct> output_iterator(
      std::cout,
      "\n");
  std::copy(
      data.begin(),
      data.end(),
      output_iterator);

  return 0;
}
