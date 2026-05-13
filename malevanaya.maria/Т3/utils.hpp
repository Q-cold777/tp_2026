#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <vector>

#include "polygon.hpp"

void cmdArea(const std::vector<Polygon>& polygons, const std::string& param);
void cmdMax(const std::vector<Polygon>& polygons, const std::string& param);
void cmdMin(const std::vector<Polygon>& polygons, const std::string& param);
void cmdCount(const std::vector<Polygon>& polygons, const std::string& param);

void cmdInframe(const std::vector<Polygon>& polygons, const Polygon& poly);
void cmdRightshapes(const std::vector<Polygon>& polygons);

bool parsePolygonArg(std::istream& in, Polygon& poly);

#endif
