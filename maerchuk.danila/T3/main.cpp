#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <cmath>
#include <iterator>
#include <functional>

struct Point
{
    int x, y;
};

struct Polygon
{
    std::vector<Point> points;
};

// --- Геометрические функции ---

double getArea(const Polygon& polygon)
{
    if (polygon.points.size() < 3) return 0.0;

    double area = std::accumulate(
        polygon.points.begin(), polygon.points.end(), 0.0,
        [&polygon](double sum, const Point& p) {
            size_t idx = &p - &polygon.points[0];
            size_t next_idx = (idx + 1) % polygon.points.size();
            const Point& next_p = polygon.points[next_idx];
            return sum + (p.x * next_p.y - next_p.x * p.y);
        }
    );
    return std::abs(area) / 2.0;
}

bool isRectangle(const Polygon& polygon)
{
    if (polygon.points.size() != 4) return false;

    const auto& p = polygon.points;

    auto v1x = p[1].x - p[0].x; auto v1y = p[1].y - p[0].y;
    auto v2x = p[2].x - p[1].x; auto v2y = p[2].y - p[1].y;
    auto v3x = p[3].x - p[2].x; auto v3y = p[3].y - p[2].y;
    auto v4x = p[0].x - p[3].x; auto v4y = p[0].y - p[3].y;

    auto dot1 = v1x * v2x + v1y * v2y;
    auto dot2 = v2x * v3x + v2y * v3y;
    auto dot3 = v3x * v4x + v3y * v4y;
    auto dot4 = v4x * v1x + v4y * v1y;

    return (dot1 == 0 && dot2 == 0 && dot3 == 0 && dot4 == 0);
}

bool onSegment(Point p, Point q, Point r) {
    return q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
           q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y);
}

int orientation(Point p, Point q, Point r) {
    int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (val == 0) return 0;
    return (val > 0) ? 1 : 2;
}

bool doSegmentsIntersect(Point p1, Point q1, Point p2, Point q2) {
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    if (o1 != o2 && o3 != o4) return true;
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;
    return false;
}

bool isPointInPolygon(Point pt, const Polygon& poly) {
    if (poly.points.size() < 3) return false;
    bool inside = false;
    size_t n = poly.points.size();
    for (size_t i = 0, j = n - 1; i < n; j = i++) {
        bool cond1 = (poly.points[i].y > pt.y) != (poly.points[j].y > pt.y);
        if (cond1) {
            long long calc_x = (poly.points[j].x - poly.points[i].x);
            calc_x = calc_x * (pt.y - poly.points[i].y);
            calc_x = calc_x / (poly.points[j].y - poly.points[i].y);
            calc_x = calc_x + poly.points[i].x;
            if (pt.x < calc_x) {
                inside = !inside;
            }
        }
    }
    return inside;
}

bool doPolygonsIntersect(const Polygon& poly1, const Polygon& poly2)
{
    bool intersect = std::any_of(
        poly1.points.begin(), poly1.points.end(), [&](const Point& p1) {
            size_t i = &p1 - &poly1.points[0];
            Point q1 = poly1.points[(i + 1) % poly1.points.size()];

            return std::any_of(
                poly2.points.begin(), poly2.points.end(), [&](const Point& p2) {
                    size_t j = &p2 - &poly2.points[0];
                    Point q2 = poly2.points[(j + 1) % poly2.points.size()];
                    return doSegmentsIntersect(p1, q1, p2, q2);
                }
            );
        }
    );
    if (intersect) return true;

    if (isPointInPolygon(poly1.points[0], poly2)) return true;
    if (isPointInPolygon(poly2.points[0], poly1)) return true;

    return false;
}

// --- Парсинг данных ---

bool parsePoint(const std::string& s, Point& pt)
{
    if (s.size() < 5 || s.front() != '(' || s.back() != ')') return false;
    std::string inner = s.substr(1, s.size() - 2);
    size_t semi = inner.find(';');
    if (semi == std::string::npos || semi == 0 || semi == inner.size() - 1)
    {
        return false;
    }

    auto check_digits = [](const std::string& str) {
        if (str.empty()) return false;
        size_t start = (str[0] == '-') ? 1 : 0;
        if (start == str.size()) return false;
        return std::all_of(str.begin() + start, str.end(), ::isdigit);
    };

    std::string x_str = inner.substr(0, semi);
    std::string y_str = inner.substr(semi + 1);

    if (!check_digits(x_str) || !check_digits(y_str)) return false;

    try {
        pt.x = std::stoi(x_str);
        pt.y = std::stoi(y_str);
    } catch (...) {
        return false;
    }
    return true;
}

bool parsePolygonLine(const std::string& line, Polygon& poly)
{
    if (line.empty()) return false;
    std::stringstream ss(line);
    size_t num_vertices;
    if (!(ss >> num_vertices)) return false;
    if (num_vertices < 3) return false;

    std::vector<std::string> tokens;
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }

    if (tokens.size() != num_vertices) return false;

    poly.points.resize(num_vertices);
    for (size_t i = 0; i < num_vertices; ++i) {
        if (!parsePoint(tokens[i], poly.points[i])) return false;
    }
    return true;
}

bool parsePolygonFromStream(std::istream& is, Polygon& poly)
{
    size_t num_vertices;
    if (!(is >> num_vertices)) return false;
    if (num_vertices < 3) return false;

    poly.points.resize(num_vertices);
    for (size_t i = 0; i < num_vertices; ++i) {
        std::string token;
        if (!(is >> token) || !parsePoint(token, poly.points[i])) return false;
    }
    return true;
}

// --- Обработчики базовых команд ---

void handleArea(const std::vector<Polygon>& polygons, std::istream& is)
{
    std::string arg;
    if (!(is >> arg)) { std::cout << "<INVALID COMMAND>\n"; return; }

    double total_area = 0.0;

    if (arg == "EVEN") {
        total_area = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [](double sum, const Polygon& p) {
                return sum + (p.points.size() % 2 == 0 ? getArea(p) : 0.0);
            });
    } else if (arg == "ODD") {
        total_area = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [](double sum, const Polygon& p) {
                return sum + (p.points.size() % 2 != 0 ? getArea(p) : 0.0);
            });
    } else if (arg == "MEAN") {
        if (polygons.empty()) { std::cout << "<INVALID COMMAND>\n"; return; }
        double sum_area = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [](double sum, const Polygon& p) { return sum + getArea(p); });
        total_area = sum_area / polygons.size();
    } else {
        try {
            long long num = std::stoll(arg);
            if (num < 3) { std::cout << "<INVALID COMMAND>\n"; return; }
            total_area = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                [num](double sum, const Polygon& p) {
                    bool match = p.points.size() == static_cast<size_t>(num);
                    return sum + (match ? getArea(p) : 0.0);
                });
        } catch (...) {
            std::cout << "<INVALID COMMAND>\n";
            return;
        }
    }
    std::cout << std::fixed << std::setprecision(1) << total_area << "\n";
}

void handleMax(const std::vector<Polygon>& polygons, std::istream& is)
{
    std::string arg;
    if (!(is >> arg) || polygons.empty()) { std::cout << "<INVALID COMMAND>\n"; return; }

    if (arg == "AREA") {
        auto it = std::max_element(polygons.begin(), polygons.end(),
            [](const Polygon& a, const Polygon& b) { return getArea(a) < getArea(b); });
        std::cout << std::fixed << std::setprecision(1) << getArea(*it) << "\n";
    } else if (arg == "VERTEXES") {
        auto it = std::max_element(polygons.begin(), polygons.end(),
            [](const Polygon& a, const Polygon& b) {
                return a.points.size() < b.points.size();
            });
        std::cout << it->points.size() << "\n";
    } else {
        std::cout << "<INVALID COMMAND>\n";
    }
}

void handleMin(const std::vector<Polygon>& polygons, std::istream& is)
{
    std::string arg;
    if (!(is >> arg) || polygons.empty()) { std::cout << "<INVALID COMMAND>\n"; return; }

    if (arg == "AREA") {
        auto it = std::min_element(polygons.begin(), polygons.end(),
            [](const Polygon& a, const Polygon& b) { return getArea(a) < getArea(b); });
        std::cout << std::fixed << std::setprecision(1) << getArea(*it) << "\n";
    } else if (arg == "VERTEXES") {
        auto it = std::min_element(polygons.begin(), polygons.end(),
            [](const Polygon& a, const Polygon& b) {
                return a.points.size() < b.points.size();
            });
        std::cout << it->points.size() << "\n";
    } else {
        std::cout << "<INVALID COMMAND>\n";
    }
}

void handleCount(const std::vector<Polygon>& polygons, std::istream& is)
{
    std::string arg;
    if (!(is >> arg)) { std::cout << "<INVALID COMMAND>\n"; return; }

    long long count = 0;
    if (arg == "EVEN") {
        count = std::count_if(polygons.begin(), polygons.end(),
            [](const Polygon& p) { return p.points.size() % 2 == 0; });
    } else if (arg == "ODD") {
        count = std::count_if(polygons.begin(), polygons.end(),
            [](const Polygon& p) { return p.points.size() % 2 != 0; });
    } else {
        try {
            long long num = std::stoll(arg);
            if (num < 3) { std::cout << "<INVALID COMMAND>\n"; return; }
            count = std::count_if(polygons.begin(), polygons.end(),
                [num](const Polygon& p) {
                    return p.points.size() == static_cast<size_t>(num);
                });
        } catch (...) {
            std::cout << "<INVALID COMMAND>\n";
            return;
        }
    }
    std::cout << count << "\n";
}

// --- Вариант 15 ---

void handleRects(const std::vector<Polygon>& polygons)
{
    long long count = std::count_if(polygons.begin(), polygons.end(), isRectangle);
    std::cout << count << "\n";
}

void handleIntersections(const std::vector<Polygon>& polygons, std::istream& is)
{
    Polygon target;
    if (!parsePolygonFromStream(is, target)) {
        std::cout << "<INVALID COMMAND>\n";
        std::string dummy;
        std::getline(is, dummy);
        return;
    }

    long long count = std::count_if(polygons.begin(), polygons.end(),
        [&](const Polygon& p) { return doPolygonsIntersect(p, target); });
    std::cout << count << "\n";
}

// --- Главная функция ---

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Error: File name is not provided.\n";
        return 1;
    }

    std::ifstream infile(argv[1]);
    if (!infile.is_open()) {
        std::cerr << "Error: Cannot open file " << argv[1] << "\n";
        return 1;
    }

    std::vector<Polygon> polygons;
    std::string line;

    while (std::getline(infile, line)) {
        Polygon poly;
        if (parsePolygonLine(line, poly)) {
            polygons.push_back(poly);
        }
    }
    infile.close();

    std::string command;
    while (std::cin >> command) {
        if (command == "AREA") {
            handleArea(polygons, std::cin);
        } else if (command == "MAX") {
            handleMax(polygons, std::cin);
        } else if (command == "MIN") {
            handleMin(polygons, std::cin);
        } else if (command == "COUNT") {
            handleCount(polygons, std::cin);
        } else if (command == "RECTS") {
            handleRects(polygons);
        } else if (command == "INTERSECTIONS") {
            handleIntersections(polygons, std::cin);
        } else {
            std::cout << "<INVALID COMMAND>\n";
            std::string dummy;
            std::getline(std::cin, dummy);
        }
    }

    return 0;
}
