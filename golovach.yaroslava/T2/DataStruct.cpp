#include "DataStruct.hpp"
#include "Delimiters.hpp"
#include <iomanip>
#include <sstream>

bool compareDataStruct(const DataStruct& lhs, const DataStruct& rhs)
{
    if (lhs.key1 != rhs.key1)
        return lhs.key1 < rhs.key1;
    if (lhs.key2 != rhs.key2)
        return lhs.key2 < rhs.key2;
    return lhs.key3.length() < rhs.key3.length();
}

static bool readToken(std::istream& in, std::string& token)
{
    token.clear();
    char c = 0;
    while (in.get(c) && c == ' ') {}
    if (!in) return false;
    token += c;
    while (in.get(c) && c != ':' && c != ' ')
        token += c;
    if (in && c == ':')
        in.putback(c);
    return !token.empty();
}

static bool parseDblSci(const std::string& token, double& val)
{
    if (token.empty()) return false;
    char last = token.back();
    if (last == 'd' || last == 'D') return false;
    size_t epos = token.find_first_of("eE");
    if (epos == std::string::npos) return false;
    std::string mantissa = token.substr(0, epos);
    if (mantissa.find('.') == std::string::npos) return false;
    size_t dotpos = mantissa.find('.');
    size_t start = (mantissa[0] == '-') ? 1 : 0;
    if (dotpos == start) return false;
    if (dotpos == mantissa.size() - 1) return false;
    std::istringstream ss(token);
    ss >> val;
    return !ss.fail() && ss.eof();
}

static bool parseRecord(std::istream& ss, DataStruct& dest)
{
    DataStruct temp{0.0, '\0', ""};
    bool has_key1 = false;
    bool has_key2 = false;
    bool has_key3 = false;

    ss >> DelimiterChar{'('};
    if (!ss) return false;

    for (int i = 0; i < 3; ++i)
    {
        ss >> DelimiterChar{':'};
        if (!ss) return false;

        std::string key_name;
        char c = 0;
        while (ss.get(c) && c != ' ' && c != ':')
            key_name += c;
        if (!ss || c == ':') return false;

        if (key_name == "key1")
        {
            if (has_key1) return false;
            std::string token;
            if (!readToken(ss, token)) return false;
            double val = 0.0;
            if (!parseDblSci(token, val)) return false;
            temp.key1 = val;
            has_key1 = true;
        }
        else if (key_name == "key2")
        {
            if (has_key2) return false;
            char q1 = 0, ch = 0, q2 = 0;
            if (!ss.get(q1) || q1 != '\'') return false;
            if (!ss.get(ch)) return false;
            if (!ss.get(q2) || q2 != '\'') return false;
            temp.key2 = ch;
            has_key2 = true;
        }
        else if (key_name == "key3")
        {
            if (has_key3) return false;
            char quote = 0;
            if (!ss.get(quote) || quote != '"') return false;
            std::string str;
            if (!std::getline(ss, str, '"')) return false;
            temp.key3 = str;
            has_key3 = true;
        }
        else
        {
            return false;
        }
    }

    ss >> DelimiterChar{':'};
    if (!ss) return false;
    ss >> DelimiterChar{')'};
    if (!ss) return false;

    if (!has_key1 || !has_key2 || !has_key3) return false;
    dest = temp;
    return true;
}

std::istream& operator>>(std::istream& in, DataStruct& dest)
{
    std::istream::sentry guard(in);
    if (!guard)
        return in;

    std::string line;
    if (!std::getline(in, line))
        return in;

    std::istringstream ss(line);
    if (!parseRecord(ss, dest))
        in.setstate(std::ios::failbit);

    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src)
{
    std::ostream::sentry guard(out);
    if (!guard)
        return out;

    std::ostringstream ss;
    ss << std::scientific << std::setprecision(1) << src.key1;
    std::string s = ss.str();
    for (char& ch : s) if (ch == 'E') ch = 'e';
    size_t epos = s.find('e');
    if (epos != std::string::npos && epos + 2 < s.size())
    {
        std::string exp_part = s.substr(epos + 2);
        size_t nz = exp_part.find_first_not_of('0');
        if (nz == std::string::npos) exp_part = "0";
        else exp_part = exp_part.substr(nz);
        s = s.substr(0, epos + 2) + exp_part;
    }

    out << "(:key1 " << s;
    out << ":key2 '" << src.key2 << "'";
    out << ":key3 \"" << src.key3 << "\":)";
    return out;
}
