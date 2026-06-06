#include "polygon.h"
#include "commands.h"
#include "iofmtguard.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <limits>
#include <sstream>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "Error: filename not specified\n"
              << "Usage: " << argv[0] << " <filename>\n";
    return 1;
  }

  std::ifstream file(argv[1]);
  if (!file.is_open())
  {
    std::cerr << "Error: cannot open file '" << argv[1] << "'\n";
    return 1;
  }

  // Шаг 1: считываем фигуры из файла — отдельным действием, до обработки команд
  std::vector< Polygon > polys;
  std::string fileLine;
  while (std::getline(file, fileLine))
  {
    std::istringstream lineStream(fileLine);
    Polygon p;
    if (lineStream >> p)
    {
      // Проверяем что в строке нет лишних токенов после полигона
      std::string extra;
      if (!(lineStream >> extra))
      {
        polys.push_back(std::move(p));
      }
    }
  }

  // Шаг 2: диспетчер команд через map + std::bind (требование задания)
  using CmdFn = std::function< void(std::istream&) >;
  std::map< std::string, CmdFn > cmds;

  namespace ph = std::placeholders;
  cmds["AREA"]          = std::bind(cmdArea,          std::cref(polys), ph::_1, std::ref(std::cout));
  cmds["MAX"]           = std::bind(cmdMax,           std::cref(polys), ph::_1, std::ref(std::cout));
  cmds["MIN"]           = std::bind(cmdMin,           std::cref(polys), ph::_1, std::ref(std::cout));
  cmds["COUNT"]         = std::bind(cmdCount,         std::cref(polys), ph::_1, std::ref(std::cout));
  cmds["LESSAREA"]      = std::bind(cmdLessArea,      std::cref(polys), ph::_1, std::ref(std::cout));
  cmds["INTERSECTIONS"] = std::bind(cmdIntersections, std::cref(polys), ph::_1, std::ref(std::cout));
  cmds["MAXSEQ"]        = std::bind(cmdMaxSeq,        std::cref(polys), ph::_1, std::ref(std::cout));

  std::string line;
  while (std::getline(std::cin, line))
  {
    if (line.empty())
    {
      continue;
    }

    std::istringstream ss(line);
    std::string cmd;
    if (!(ss >> cmd))
    {
      continue;
    }

    auto it = cmds.find(cmd);
    if (it == cmds.end())
    {
      std::cout << "<INVALID COMMAND>\n";
      continue;
    }

    try
    {
      it->second(ss);

      // Проверяем лишние аргументы после обработки команды
      std::string extra;
      if (ss >> extra)
      {
        throw std::runtime_error("Too many arguments");
      }
    }
    catch (const std::exception&)
    {
      std::cout << "<INVALID COMMAND>\n";
    }
  }

  return 0;
}
