#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <limits>
#include "DataStruct.hpp"

int main()
{
    std::vector<DataStruct> data;

    while (!std::cin.eof())
    {
        DataStruct ds;
        if (std::cin >> ds)
        {
            data.push_back(ds);
        }
        else if (!std::cin.eof())
        {
            // Сбрасываем ошибку и пропускаем строку до следующей записи
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    std::sort(data.begin(), data.end(), compareDataStruct);

    std::ostream_iterator<DataStruct> output_iterator(std::cout, "\n");
    std::copy(data.begin(), data.end(), output_iterator);

    return 0;
}
