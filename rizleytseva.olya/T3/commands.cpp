#include "commands.h"
#include "iofmtguard.hpp"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <string>
#include <stdexcept>

// ============ AREA ============
void cmdArea(const std::vector< Polygon >& polys,
             std::istream& is, std::ostream& os)
{
  std::string param;
  if (!(is >> param))
  {
    throw std::runtime_error("Missing argument for AREA");
  }

  iofmtguard guard(os);
  os << std::fixed << std::setprecision(1);

  if (param == "EVEN")
  {
    double sum = std::accumulate(polys.begin(), polys.end(), 0.0,
                                 ConditionalAreaAccumulator(true));
    os << sum << '\n';
  }
  else if (param == "ODD")
  {
    double sum = std::accumulate(polys.begin(), polys.end(), 0.0,
                                 ConditionalAreaAccumulator(false));
    os << sum << '\n';
  }
  else if (param == "MEAN")
  {
    if (polys.empty())
    {
      throw std::runtime_error("Empty dataset for AREA MEAN");
    }
    double sum = std::accumulate(polys.begin(), polys.end(), 0.0,
                                 AreaAccumulator{});
    os << sum / static_cast< double >(polys.size()) << '\n';
  }
  else
  {
    std::size_t n = 0;
    try
    {
      n = std::stoull(param);
    }
    catch (...)
    {
      throw std::runtime_error("Unknown argument for AREA");
    }
    if (n < 3)
    {
      throw std::runtime_error("Vertex count must be >= 3");
    }
    double sum = std::accumulate(polys.begin(), polys.end(), 0.0,
                                 VertexCountAreaAccumulator(n));
    os << sum << '\n';
  }
}

// ============ MAX ============
void cmdMax(const std::vector< Polygon >& polys,
            std::istream& is, std::ostream& os)
{
  if (polys.empty())
  {
    throw std::runtime_error("Empty collection for MAX");
  }

  std::string param;
  if (!(is >> param))
  {
    throw std::runtime_error("Missing argument for MAX");
  }

  if (param == "AREA")
  {
    auto it = std::max_element(polys.begin(), polys.end(), AreaLess{});
    iofmtguard guard(os);
    os << std::fixed << std::setprecision(1) << getArea(*it) << '\n';
  }
  else if (param == "VERTEXES")
  {
    auto it = std::max_element(polys.begin(), polys.end(), VertexCountLess{});
    os << it->points.size() << '\n';
  }
  else
  {
    throw std::runtime_error("Unknown argument for MAX");
  }
}

// ============ MIN ============
void cmdMin(const std::vector< Polygon >& polys,
            std::istream& is, std::ostream& os)
{
  if (polys.empty())
  {
    throw std::runtime_error("Empty collection for MIN");
  }

  std::string param;
  if (!(is >> param))
  {
    throw std::runtime_error("Missing argument for MIN");
  }

  if (param == "AREA")
  {
    auto it = std::min_element(polys.begin(), polys.end(), AreaLess{});
    iofmtguard guard(os);
    os << std::fixed << std::setprecision(1) << getArea(*it) << '\n';
  }
  else if (param == "VERTEXES")
  {
    auto it = std::min_element(polys.begin(), polys.end(), VertexCountLess{});
    os << it->points.size() << '\n';
  }
  else
  {
    throw std::runtime_error("Unknown argument for MIN");
  }
}

// ============ COUNT ============
void cmdCount(const std::vector< Polygon >& polys,
              std::istream& is, std::ostream& os)
{
  std::string param;
  if (!(is >> param))
  {
    throw std::runtime_error("Missing argument for COUNT");
  }

  if (param == "EVEN")
  {
    os << std::count_if(polys.begin(), polys.end(), EvenVertexPred{}) << '\n';
  }
  else if (param == "ODD")
  {
    os << std::count_if(polys.begin(), polys.end(), OddVertexPred{}) << '\n';
  }
  else
  {
    std::size_t n = 0;
    try
    {
      n = std::stoull(param);
    }
    catch (...)
    {
      throw std::runtime_error("Unknown argument for COUNT");
    }
    if (n < 3)
    {
      throw std::runtime_error("Vertex count must be >= 3");
    }
    os << std::count_if(polys.begin(), polys.end(), VertexCountPred(n)) << '\n';
  }
}

// ============ LESSAREA ============
void cmdLessArea(const std::vector< Polygon >& polys,
                 std::istream& is, std::ostream& os)
{
  Polygon target;
  if (!(is >> target))
  {
    throw std::runtime_error("Invalid polygon for LESSAREA");
  }
  double targetArea = getArea(target);
  os << std::count_if(polys.begin(), polys.end(), AreaLessThan(targetArea)) << '\n';
}

// ============ INTERSECTIONS ============
void cmdIntersections(const std::vector< Polygon >& polys,
                      std::istream& is, std::ostream& os)
{
  Polygon target;
  if (!(is >> target))
  {
    throw std::runtime_error("Invalid polygon for INTERSECTIONS");
  }
  os << std::count_if(polys.begin(), polys.end(), IntersectsWith(target)) << '\n';
}

// ============ MAXSEQ ============
// Считает максимальное количество идущих подряд фигур, идентичных target.
// Используем std::adjacent_find / accumulate нельзя напрямую — применяем
// transform + accumulate: преобразуем вектор в 0/1, затем считаем max run.
// Без циклов и лямбд — через функтор MaxRunAccumulator.

// Пара (текущая_серия, максимальная_серия)
struct MaxRunState
{
  long long current;
  long long maxRun;
};

// Функтор для std::accumulate: накапливает длину серии совпадений
struct MaxRunAccumulator
{
  const Polygon& target;

  explicit MaxRunAccumulator(const Polygon& t) : target(t) {}

  MaxRunState operator()(MaxRunState state, const Polygon& p) const
  {
    if (p == target)
    {
      state.current++;
      if (state.current > state.maxRun)
      {
        state.maxRun = state.current;
      }
    }
    else
    {
      state.current = 0;
    }
    return state;
  }
};

void cmdMaxSeq(const std::vector< Polygon >& polys,
               std::istream& is, std::ostream& os)
{
  Polygon target;
  if (!(is >> target))
  {
    os << "<INVALID COMMAND>\n";
    return;
  }

  MaxRunState result = std::accumulate(
    polys.begin(), polys.end(),
    MaxRunState{ 0, 0 },
    MaxRunAccumulator(target)
  );

  os << result.maxRun << '\n';
}
