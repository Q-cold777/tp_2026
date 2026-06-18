#include "DataStruct.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <limits>

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
            std::cin.clear();
        }
    }

    std::sort(data.begin(), data.end(), compareDataStruct);

    std::copy(
        data.begin(),
        data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );

    return 0;
}
