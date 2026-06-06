#include "shapes.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <climits>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: cannot open file " << argv[1] << "\n";
        return 1;
    }

    std::vector<Polygon> polygons;
    Polygon p;
    while (file >> p) {
        polygons.push_back(p);
    }
    file.close();

    std::cout << std::fixed << std::setprecision(1);

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "AREA") {
            std::string sub;
            iss >> sub;
            if (sub == "EVEN") {
                double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                                             [](double acc, const Polygon& p) { return p.points.size() % 2 == 0 ? acc + area(p) : acc; });
                std::cout << sum << "\n";
            } else if (sub == "ODD") {
                double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                                             [](double acc, const Polygon& p) { return p.points.size() % 2 != 0 ? acc + area(p) : acc; });
                std::cout << sum << "\n";
            } else if (sub == "MEAN") {
                if (polygons.empty()) {
                    std::cout << "<INVALID COMMAND>\n";
                } else {
                    double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                                                 [](double acc, const Polygon& p) { return acc + area(p); });
                    std::cout << sum / polygons.size() << "\n";
                }
            } else {
                try {
                    int n = std::stoi(sub);
                    if (n < 3) {
                        std::cout << "<INVALID COMMAND>\n";
                    } else {
                        double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                                                     [n](double acc, const Polygon& p) { return p.points.size() == static_cast<size_t>(n) ? acc + area(p) : acc; });
                        std::cout << sum << "\n";
                    }
                } catch (...) {
                    std::cout << "<INVALID COMMAND>\n";
                }
            }
        }
        else if (cmd == "MAX" || cmd == "MIN") {
            std::string sub;
            iss >> sub;
            if (polygons.empty()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }
            if (sub == "AREA") {
                auto cmp = [](const Polygon& a, const Polygon& b) { return area(a) < area(b); };
                auto it = (cmd == "MAX") ? std::max_element(polygons.begin(), polygons.end(), cmp)
                : std::min_element(polygons.begin(), polygons.end(), cmp);
                std::cout << area(*it) << "\n";
            } else if (sub == "VERTEXES") {
                auto cmp = [](const Polygon& a, const Polygon& b) { return a.points.size() < b.points.size(); };
                auto it = (cmd == "MAX") ? std::max_element(polygons.begin(), polygons.end(), cmp)
                : std::min_element(polygons.begin(), polygons.end(), cmp);
                std::cout << it->points.size() << "\n";
            } else {
                std::cout << "<INVALID COMMAND>\n";
            }
        }
        else if (cmd == "COUNT") {
            std::string sub;
            iss >> sub;
            if (sub == "EVEN") {
                std::cout << std::count_if(polygons.begin(), polygons.end(),
                                           [](const Polygon& p) { return p.points.size() % 2 == 0; }) << "\n";
            } else if (sub == "ODD") {
                std::cout << std::count_if(polygons.begin(), polygons.end(),
                                           [](const Polygon& p) { return p.points.size() % 2 != 0; }) << "\n";
            } else {
                try {
                    int n = std::stoi(sub);
                    if (n < 3) {
                        std::cout << "<INVALID COMMAND>\n";
                    } else {
                        std::cout << std::count_if(polygons.begin(), polygons.end(),
                                                   [n](const Polygon& p) { return p.points.size() == static_cast<size_t>(n); }) << "\n";
                    }
                } catch (...) {
                    std::cout << "<INVALID COMMAND>\n";
                }
            }
        }
        else if (cmd == "RMECHO") {
            Polygon target;
            std::string target_str;
            std::getline(iss >> std::ws, target_str);

            std::istringstream target_iss(target_str);
            if (!(target_iss >> target)) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            auto new_end = std::unique(polygons.begin(), polygons.end(),
                                       [&](const Polygon& a, const Polygon& b) { return a == target && b == target; });
            int removed = std::distance(new_end, polygons.end());
            polygons.erase(new_end, polygons.end());
            std::cout << removed << "\n";
        }
        else if (cmd == "INFRAME") {
            Polygon target;
            std::string target_str;
            std::getline(iss >> std::ws, target_str);

            std::istringstream target_iss(target_str);
            if (!(target_iss >> target)) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (polygons.empty()) {
                std::cout << "<FALSE>\n";
                continue;
            }

            int global_min_x = INT_MAX, global_max_x = INT_MIN;
            int global_min_y = INT_MAX, global_max_y = INT_MIN;

            for (const auto& poly : polygons) {
                for (const auto& pt : poly.points) {
                    global_min_x = std::min(global_min_x, pt.x);
                    global_max_x = std::max(global_max_x, pt.x);
                    global_min_y = std::min(global_min_y, pt.y);
                    global_max_y = std::max(global_max_y, pt.y);
                }
            }

            int t_min_x = INT_MAX, t_max_x = INT_MIN;
            int t_min_y = INT_MAX, t_max_y = INT_MIN;

            for (const auto& pt : target.points) {
                t_min_x = std::min(t_min_x, pt.x);
                t_max_x = std::max(t_max_x, pt.x);
                t_min_y = std::min(t_min_y, pt.y);
                t_max_y = std::max(t_max_y, pt.y);
            }

            bool inside = (t_min_x >= global_min_x) &&
            (t_max_x <= global_max_x) &&
            (t_min_y >= global_min_y) &&
            (t_max_y <= global_max_y);

            std::cout << (inside ? "<TRUE>" : "<FALSE>") << "\n";
        }
        else {
            std::cout << "<INVALID COMMAND>\n";
        }
    }

    return 0;
}
