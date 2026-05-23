#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <limits>
#include <cmath>
#include <fstream>
#include "dataStruct.hpp"

int main()
{
  std::vector<DataStruct> data;

  // Открываем файл для чтения
  std::ifstream infile("input.txt");

  if (!infile.is_open())
  {
    std::cerr << "Error: could not open input.txt\n";
    return 1;
  }

  // 1. Построчное чтение данных из файла
  while (!infile.eof())
  {
    std::copy(
        std::istream_iterator<DataStruct>(infile),
        std::istream_iterator<DataStruct>(),
        std::back_inserter(data));

    if (infile.fail() && !infile.eof())
    {
      infile.clear();
      infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  }

  infile.close();

  // 2. Сортировка по новым критериям (key1 — комплексное, key2 — double)
  std::sort(data.begin(), data.end(), [](const DataStruct &a, const DataStruct &b)
            {
    const double EPS = 1e-9; // Погрешность сравнения double

    // 1. Сначала по возрастанию модуля комплексного числа key1
    double absA1 = std::abs(a.key1);
    double absB1 = std::abs(b.key1);
    if (std::abs(absA1 - absB1) > EPS) {
      return absA1 < absB1;
    }

    // 2. Если модули key1 равны, сравниваем по возрастанию key2 (double)
    if (std::abs(a.key2 - b.key2) > EPS) {
      return a.key2 < b.key2;
    }

    // 3. Если и тут равны, сравниваем по длине строки key3
    return a.key3.length() < b.key3.length(); });

  // 3. Вывод отсортированных данных в консоль
  std::copy(
      data.begin(),
      data.end(),
      std::ostream_iterator<DataStruct>(std::cout, "\n"));

  return 0;
}
