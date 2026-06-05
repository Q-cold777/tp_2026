#include "data_struct.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <complex>
#include <cmath>

bool parseDouble(const std::string& s, double& value) {
    std::string str = s;
    // Убираем суффиксы d/D/e/E
    if (!str.empty() && (str.back() == 'd' || str.back() == 'D')) str.pop_back();
    size_t pos = 0;
    try {
        value = std::stod(str, &pos);
        return pos == str.size();
    } catch (...) { return false; }
}

bool parseLongLong(const std::string& s, long long& value) {
    if (s.empty()) return false;
    std::string str = s;
    // Убираем суффикс LL/ll
    if (str.size() >= 2 && (str.substr(str.size()-2) == "LL" || str.substr(str.size()-2) == "ll")) {
        str = str.substr(0, str.size()-2);
    }
    size_t pos = 0;
    try {
        value = std::stoll(str, &pos);
        return pos == str.size();
    } catch (...) { return false; }
}

bool parseUnsignedLongLong(const std::string& s, unsigned long long& value) {
    if (s.empty()) return false;
    std::string str = s;
    // Убираем суффикс ULL/ull/u11
    if (str.size() >= 3 && (str.substr(str.size()-3) == "ULL" || str.substr(str.size()-3) == "ull")) {
        str = str.substr(0, str.size()-3);
    } else if (str.size() >= 1 && (str.back() == 'u' || str.back() == 'U')) {
        str.pop_back();
    }
    // Восьмеричное
    if (str.size() > 1 && str[0] == '0' && str[1] != 'x' && str[1] != 'X' && str[1] != 'b' && str[1] != 'B') {
        try {
            value = std::stoull(str, nullptr, 8);
            return true;
        } catch (...) { return false; }
    }
    // Двоичное
    if (str.size() > 2 && str[0] == '0' && (str[1] == 'b' || str[1] == 'B')) {
        value = 0;
        for (size_t i = 2; i < str.size(); ++i) {
            char c = str[i];
            if (c == '0') value = value * 2;
            else if (c == '1') value = value * 2 + 1;
            else return false;
        }
        return true;
    }
    // Шестнадцатеричное
    if (str.size() > 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        try {
            value = std::stoull(str, nullptr, 16);
            return true;
        } catch (...) { return false; }
    }
    // Десятичное
    try {
        value = std::stoull(str);
        return true;
    } catch (...) { return false; }
}

bool parseChar(const std::string& s, char& value) {
    if (s.size() == 3 && s[0] == '\'' && s[2] == '\'') {
        value = s[1];
        return true;
    }
    return false;
}

bool parseComplex(const std::string& s, std::complex<double>& value) {
    if (s.size() < 6 || s.substr(0,3) != "#c(" || s.back() != ')') return false;
    std::string inner = s.substr(3, s.size() - 4);
    size_t space = inner.find(' ');
    if (space == std::string::npos) return false;
    double real, imag;
    if (!parseDouble(inner.substr(0, space), real)) return false;
    if (!parseDouble(inner.substr(space+1), imag)) return false;
    value = std::complex<double>(real, imag);
    return true;
}

bool parseRational(const std::string& s, std::pair<long long, unsigned long long>& value) {
    if (s.size() < 6 || s.substr(0,3) != "(:N" || s.back() != ':' ) return false;
    size_t dPos = s.find(":D");
    if (dPos == std::string::npos) return false;
    std::string numStr = s.substr(3, dPos - 3);
    std::string denStr = s.substr(dPos+2, s.size() - dPos - 3);
    long long num;
    unsigned long long den;
    if (!parseLongLong(numStr, num)) return false;
    if (!parseUnsignedLongLong(denStr, den)) return false;
    value = {num, den};
    return true;
}

std::istream& operator>>(std::istream& in, DataStruct& data) {
    std::string line;
    if (!std::getline(in, line)) return in;

    size_t start = line.find_first_not_of(" \t");
    size_t end = line.find_last_not_of(" \t");
    if (start == std::string::npos || end == std::string::npos) {
        in.setstate(std::ios::failbit);
        return in;
    }
    line = line.substr(start, end - start + 1);

    if (line.size() < 4 || line[0] != '(' || line[1] != ':' || line.back() != ':') {
        in.setstate(std::ios::failbit);
        return in;
    }

    line = line.substr(2, line.size() - 3);

    bool hasKey1 = false, hasKey2 = false, hasKey3 = false;
    char k1 = 0;
    unsigned long long k2 = 0;
    std::string k3;

    size_t pos = 0;
    while (pos < line.size()) {
        if (line[pos] != ':') {
            in.setstate(std::ios::failbit);
            return in;
        }
        pos++;
        size_t nextPos = line.find(':', pos);
        if (nextPos == std::string::npos) {
            in.setstate(std::ios::failbit);
            return in;
        }
        std::string part = line.substr(pos, nextPos - pos);
        pos = nextPos;

        size_t spacePos = part.find(' ');
        if (spacePos == std::string::npos) {
            in.setstate(std::ios::failbit);
            return in;
        }
        std::string keyName = part.substr(0, spacePos);
        std::string keyValue = part.substr(spacePos + 1);

        if (keyName == "key1") {
            // Парсим key1 как char
            if (keyValue.size() == 3 && keyValue[0] == '\'' && keyValue[2] == '\'') {
                k1 = keyValue[1];
                hasKey1 = true;
            }
            // Если не char, пробуем как unsigned long long (hex)
            else if (keyValue.size() > 2 && keyValue[0] == '0' && (keyValue[1] == 'x' || keyValue[1] == 'X')) {
                unsigned long long val = 0;
                bool ok = true;
                for (size_t i = 2; i < keyValue.size(); ++i) {
                    char c = keyValue[i];
                    if (c >= '0' && c <= '9') val = val * 16 + (c - '0');
                    else if (c >= 'A' && c <= 'F') val = val * 16 + (c - 'A' + 10);
                    else if (c >= 'a' && c <= 'f') val = val * 16 + (c - 'a' + 10);
                    else { ok = false; break; }
                }
                if (ok) {
                    k1 = static_cast<char>(val);
                    hasKey1 = true;
                }
            }
            if (!hasKey1) {
                in.setstate(std::ios::failbit);
                return in;
            }
        }
        else if (keyName == "key2") {
            // Парсим key2 как unsigned long long (hex)
            if (keyValue.size() > 2 && keyValue[0] == '0' && (keyValue[1] == 'x' || keyValue[1] == 'X')) {
                unsigned long long val = 0;
                bool ok = true;
                for (size_t i = 2; i < keyValue.size(); ++i) {
                    char c = keyValue[i];
                    if (c >= '0' && c <= '9') val = val * 16 + (c - '0');
                    else if (c >= 'A' && c <= 'F') val = val * 16 + (c - 'A' + 10);
                    else if (c >= 'a' && c <= 'f') val = val * 16 + (c - 'a' + 10);
                    else { ok = false; break; }
                }
                if (ok) {
                    k2 = val;
                    hasKey2 = true;
                }
            }
            if (!hasKey2) {
                in.setstate(std::ios::failbit);
                return in;
            }
        }
        else if (keyName == "key3") {
            if (keyValue.size() >= 2 && keyValue[0] == '"' && keyValue.back() == '"') {
                k3 = keyValue.substr(1, keyValue.size() - 2);
                hasKey3 = true;
            } else {
                in.setstate(std::ios::failbit);
                return in;
            }
        }
    }

    if (!hasKey1 || !hasKey2 || !hasKey3) {
        in.setstate(std::ios::failbit);
        return in;
    }

    data.key1 = k1;
    data.key2 = k2;
    data.key3 = k3;
    return in;
}
