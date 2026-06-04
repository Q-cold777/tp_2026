#include "utils.hpp"
#include <sstream>
#include <fstream>
#include <algorithm>

bool readPoint(std::istream& is, Point& pt) {
    char c1, c2, c3;
    if (is >> c1 >> pt.x >> c2 >> pt.y >> c3) {
        if (c1 == '(' && c2 == ';' && c3 == ')') {
            return true;
        }
    }
    return false;
}

bool parsePolygon(const std::string& line, Polygon& polygon) {
    std::stringstream ss(line);
    size_t vertexesCount;
    if (!(ss >> vertexesCount)) return false;

    polygon.points.clear();
    bool success = true;
    polygon.points.resize(vertexesCount);

    std::generate_n(polygon.points.begin(), vertexesCount, [&]() {
        Point pt;
        if (!readPoint(ss, pt)) {
            success = false;
        }
        return pt;
    });

    std::string trailing;
    if (ss >> trailing || !success || polygon.points.size() != vertexesCount) {
        return false;
    }
    return true;
}

std::vector<Polygon> loadPolygonsFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<Polygon> polygons;
    if (!file.is_open()) return polygons;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        Polygon poly;
        if (parsePolygon(line, poly)) {
            polygons.push_back(poly);
        }
    }
    return polygons;
}
