#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <regex>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <optional>

struct Point {
    int x, y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

struct Polygon {
    std::vector<Point> points;
    double area = 0.0;
    bool operator==(const Polygon& other) const {
        if (points.size() != other.points.size()) return false;
        return std::equal(points.begin(), points.end(), other.points.begin());
    }
};

// Вычисление площади (формула Гаусса) без циклов
double calculateArea(const std::vector<Point>& points) {
    if (points.size() < 3) return 0.0;
    std::vector<size_t> indices(points.size());
    std::iota(indices.begin(), indices.end(), 0);

    double area = std::accumulate(indices.begin(), indices.end(), 0.0,
        [&](double acc, size_t i) {
            size_t next = (i + 1) % points.size();
            return acc + (points[i].x * points[next].y - points[next].x * points[i].y);
        });
    return std::abs(area) / 2.0;
}

// Проверка наличия прямого угла без циклов
bool hasRightAngle(const Polygon& polygon) {
    if (polygon.points.size() < 3) return false;
    std::vector<size_t> indices(polygon.points.size());
    std::iota(indices.begin(), indices.end(), 0);

    return std::any_of(indices.begin(), indices.end(), [&](size_t i) {
        size_t prev = (i == 0) ? polygon.points.size() - 1 : i - 1;
        size_t next = (i + 1) % polygon.points.size();
        long long dx1 = polygon.points[i].x - polygon.points[prev].x;
        long long dy1 = polygon.points[i].y - polygon.points[prev].y;
        long long dx2 = polygon.points[next].x - polygon.points[i].x;
        long long dy2 = polygon.points[next].y - polygon.points[i].y;
        return dx1 * dx2 + dy1 * dy2 == 0;
    });
}

// Парсинг строки в фигуру
std::optional<Polygon> parsePolygon(const std::string& line) {
    static const std::regex re(R"(^\s*(\d+)\s*((?:\s*\(-?\d+;-?\d+\)\s*)+)\s*$)");
    static const std::regex pointRe(R"(\(\s*(-?\d+)\s*;\s*(-?\d+)\s*\))");

    std::smatch match;
    if (std::regex_match(line, match, re)) {
        int n = std::stoi(match[1].str());
        std::string coords = match[2].str();

        auto points_begin = std::sregex_iterator(coords.begin(), coords.end(), pointRe);
        auto points_end = std::sregex_iterator();

        std::vector<Point> points;
        points.reserve(n);

        std::transform(points_begin, points_end, std::back_inserter(points),
            [](const std::smatch& m) {
                return Point{std::stoi(m[1].str()), std::stoi(m[2].str())};
            });

        if (points.size() == static_cast<size_t>(n)) {
            Polygon p;
            p.points = std::move(points);
            p.area = calculateArea(p.points);
            return p;
        }
    }
    return std::nullopt;
}

// Кастомный итератор для команды ECHO (позволяет избежать циклов)
struct EchoInserter {
    std::vector<Polygon>& vec;
    const Polygon& target;
    int& count;

    EchoInserter& operator=(const Polygon& p) {
        vec.push_back(p);
        if (p == target) {
            vec.push_back(p);
            count++;
        }
        return *this;
    }
    EchoInserter& operator*() { return *this; }
    EchoInserter& operator++() { return *this; }
    EchoInserter& operator++(int) { return *this; }
};

void processCommand(const std::string& line, std::vector<Polygon>& polygons) {
    std::stringstream ss(line);
    std::string cmd, arg1;
    if (!(ss >> cmd)) return;

    bool valid = true;

    if (cmd == "AREA") {
        if (ss >> arg1) {
            if (arg1 == "EVEN") {
                double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                    [](double acc, const Polygon& p) { return acc + (p.points.size() % 2 == 0 ? p.area : 0.0); });
                std::cout << sum << "\n";
            } else if (arg1 == "ODD") {
                double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                    [](double acc, const Polygon& p) { return acc + (p.points.size() % 2 != 0 ? p.area : 0.0); });
                std::cout << sum << "\n";
            } else if (arg1 == "MEAN") {
                if (polygons.empty()) std::cout << 0.0 << "\n";
                else {
                    double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                        [](double acc, const Polygon& p) { return acc + p.area; });
                    std::cout << (sum / polygons.size()) << "\n";
                }
            } else {
                try {
                    int num = std::stoi(arg1);
                    double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                        [num](double acc, const Polygon& p) { return acc + (p.points.size() == static_cast<size_t>(num) ? p.area : 0.0); });
                    std::cout << sum << "\n";
                } catch (...) { valid = false; }
            }
        } else { valid = false; }
    }
    else if (cmd == "MAX") {
        if (ss >> arg1) {
            if (polygons.empty()) { std::cout << (arg1 == "AREA" ? 0.0 : 0) << "\n"; return; }
            if (arg1 == "AREA") {
                auto it = std::max_element(polygons.begin(), polygons.end(), [](const Polygon& a, const Polygon& b) { return a.area < b.area; });
                std::cout << it->area << "\n";
            } else if (arg1 == "VERTEXES") {
                auto it = std::max_element(polygons.begin(), polygons.end(), [](const Polygon& a, const Polygon& b) { return a.points.size() < b.points.size(); });
                std::cout << it->points.size() << "\n";
            } else { valid = false; }
        } else { valid = false; }
    }
    else if (cmd == "MIN") {
        if (ss >> arg1) {
            if (polygons.empty()) { std::cout << (arg1 == "AREA" ? 0.0 : 0) << "\n"; return; }
            if (arg1 == "AREA") {
                auto it = std::min_element(polygons.begin(), polygons.end(), [](const Polygon& a, const Polygon& b) { return a.area < b.area; });
                std::cout << it->area << "\n";
            } else if (arg1 == "VERTEXES") {
                auto it = std::min_element(polygons.begin(), polygons.end(), [](const Polygon& a, const Polygon& b) { return a.points.size() < b.points.size(); });
                std::cout << it->points.size() << "\n";
            } else { valid = false; }
        } else { valid = false; }
    }
    else if (cmd == "COUNT") {
        if (ss >> arg1) {
            if (arg1 == "EVEN") {
                int count = std::count_if(polygons.begin(), polygons.end(), [](const Polygon& p) { return p.points.size() % 2 == 0; });
                std::cout << count << "\n";
            } else if (arg1 == "ODD") {
                int count = std::count_if(polygons.begin(), polygons.end(), [](const Polygon& p) { return p.points.size() % 2 != 0; });
                std::cout << count << "\n";
            } else {
                try {
                    int num = std::stoi(arg1);
                    int count = std::count_if(polygons.begin(), polygons.end(), [num](const Polygon& p) { return p.points.size() == static_cast<size_t>(num); });
                    std::cout << count << "\n";
                } catch (...) { valid = false; }
            }
        } else { valid = false; }
    }
    else if (cmd == "ECHO") {
        std::string rest;
        std::getline(ss, rest);
        rest.erase(0, rest.find_first_not_of(" \t"));
        if (!rest.empty()) rest.erase(rest.find_last_not_of(" \t") + 1);

        auto target_opt = parsePolygon(rest);
        if (target_opt) {
            Polygon target = *target_opt;
            std::vector<Polygon> new_polygons;
            new_polygons.reserve(polygons.size() * 2);
            int count = 0;

            // Используем std::copy с нашим итератором вместо цикла
            std::copy(polygons.begin(), polygons.end(), EchoInserter{new_polygons, target, count});
            polygons = std::move(new_polygons);
            std::cout << count << "\n";
        } else { valid = false; }
    }
    else if (cmd == "RIGHTSHAPES") {
        int count = std::count_if(polygons.begin(), polygons.end(), [](const Polygon& p) { return hasRightAngle(p); });
        std::cout << count << "\n";
    }
    else {
        valid = false;
    }

    if (!valid) {
        std::cout << "<INVALID COMMAND>\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: filename not provided\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open file\n";
        return 1;
    }

    std::vector<Polygon> polygons;
    std::string line;

    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        if (!line.empty() && line.back() == '\r') line.pop_back();
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        if (line.empty()) continue;

        auto p_opt = parsePolygon(line);
        if (p_opt) {
            polygons.push_back(std::move(*p_opt));
        }
    }
    file.close();

    std::cout << std::fixed << std::setprecision(1);

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        processCommand(line, polygons);
    }

    return 0;
}
