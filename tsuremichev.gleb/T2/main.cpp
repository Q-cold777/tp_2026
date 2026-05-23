#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <limits>
#include "dataStruct.hpp"

int main() {
  std::vector<DataStruct> data;

  // 1. Чтение данных
  while (!std::cin.eof()) {
    // Используем std::copy для чтения блока данных до первой ошибки
    std::copy(
      std::istream_iterator<DataStruct>(std::cin),
              std::istream_iterator<DataStruct>(),
              std::back_inserter(data)
    );

    // Если чтение прервалось не из-за конца файла, значит встретили неверный формат
    if (std::cin.fail() && !std::cin.eof()) {
      std::cin.clear(); // Сбрасываем флаг ошибки
      // Пропускаем "плохую" строку до конца (согласно требованию игнорировать их)
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  }

  // 2. Сортировка (согласно пункту 2 в задании)
  std::sort(data.begin(), data.end(), [](const DataStruct& a, const DataStruct& b) {
    // 1. Сначала по key1 (double)
    if (a.key1 != b.key1) {
      return a.key1 < b.key1;
    }

    // 2. Если key1 равны, сравниваем по key2 (модуль комплексного числа)
    // Используем std::abs для получения модуля
    double absA = std::abs(a.key2);
    double absB = std::abs(b.key2);
    if (absA != absB) {
      return absA < absB;
    }

    // 3. Если и тут равны, сравниваем по длине строки key3
    return a.key3.length() < b.key3.length();
  });


  // 3. Вывод данных (пункт 3 задания)
  std::copy(
    data.begin(),
            data.end(),
            std::ostream_iterator<DataStruct>(std::cout, "\n")
  );

  return 0;
}
