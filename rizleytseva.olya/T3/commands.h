#ifndef COMMANDS_H
#define COMMANDS_H

#include "polygon.h"
#include <vector>
#include <iostream>
#include <stdexcept>

// Каждая функция читает параметры из is, выводит результат в os.
// При ошибке бросает std::runtime_error — main его перехватывает
// и выводит <INVALID COMMAND>.

void cmdArea(const std::vector< Polygon >& polys,
             std::istream& is, std::ostream& os);

void cmdMax(const std::vector< Polygon >& polys,
            std::istream& is, std::ostream& os);

void cmdMin(const std::vector< Polygon >& polys,
            std::istream& is, std::ostream& os);

void cmdCount(const std::vector< Polygon >& polys,
              std::istream& is, std::ostream& os);

void cmdLessArea(const std::vector< Polygon >& polys,
                 std::istream& is, std::ostream& os);

void cmdIntersections(const std::vector< Polygon >& polys,
                      std::istream& is, std::ostream& os);

void cmdMaxSeq(const std::vector< Polygon >& polys,
               std::istream& is, std::ostream& os);

#endif // COMMANDS_H
