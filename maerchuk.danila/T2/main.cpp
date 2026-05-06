#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <limits>
#include "data_struct.hpp"

int main()
{
  using nspace::DataStruct;
  std::vector< DataStruct > data;

  // Чтение данных с использованием итераторов потока и алгоритма std::copy
  while (!std::cin.eof())
  {
    if (!std::cin)
    {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    }
    std::copy(
      std::istream_iterator< DataStruct >(std::cin),
      std::istream_iterator< DataStruct >(),
      std::back_inserter(data)
    );
  }

  std::sort(data.begin(), data.end());

  // Вывод результатов осуществляется только если вектор не пуст.
  if (!data.empty())
  {

    std::copy(
      std::begin(data),
      std::end(data),
      std::ostream_iterator< DataStruct >(std::cout, "\n")
    );
  }

  return 0;
}
