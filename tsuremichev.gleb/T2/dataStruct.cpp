#include "dataStruct.hpp"
#include <iomanip>
#include <sstream>
#include <cmath>

std::istream &operator>>(std::istream &in, DelimiterIO &&dest)
{
  std::istream::sentry sentry(in);
  if (!sentry)
    return in;
  char c;
  if (in.get(c))
  {
    if (c != dest.expected)
    {
      in.setstate(std::ios::failbit);
    }
  }
  return in;
}

std::istream &operator>>(std::istream &in, LabelIO &&dest)
{
  std::istream::sentry sentry(in);
  if (!sentry)
    return in;
  std::string actual;
  char c;
  for (size_t i = 0; i < dest.expected.length(); ++i)
  {
    if (in.get(c))
    {
      actual += c;
    }
  }
  if (actual != dest.expected)
  {
    in.setstate(std::ios::failbit);
  }
  return in;
}

std::istream &operator>>(std::istream &in, DataStruct &dest)
{
  std::string line;
  if (!std::getline(in, line))
    return in;
  if (line.empty())
    return in;

  std::stringstream ss(line);
  ss >> std::noskipws;

  char open1, open2;
  if (!(ss >> open1 >> open2) || open1 != '(' || open2 != ':')
  {
    in.setstate(std::ios::failbit);
    return in;
  }

  DataStruct input;
  bool has_k1 = false, has_k2 = false, has_k3 = false;

  for (int i = 0; i < 3; ++i)
  {
    // Пропускаем возможные пробелы перед словом "key" (например, после двоеточия)
    ss >> std::ws;

    std::string key_base;
    char ch;
    // Читаем само слово "key"
    for (int j = 0; j < 3; ++j)
    {
      if (ss.get(ch))
        key_base += ch;
    }

    if (key_base != "key")
    {
      in.setstate(std::ios::failbit);
      return in;
    }

    // Пропускаем пробел внутри имени ключа и читаем цифру (1, 2 или 3)
    char num;
    if (!(ss >> std::ws >> num))
      return in;

    if (num == '1')
    {
      double re, im;
      if (ss >> std::ws >> LabelIO{"#c("} >> std::ws >> re >> std::ws >> im >> DelimiterIO{')'})
      {
        input.key1 = {re, im};
        has_k1 = true;
      }
    }
    else if (num == '2')
    {
      if (ss >> std::ws >> input.key2)
      {
        has_k2 = true;
      }
    }
    else if (num == '3')
    {
      if (ss >> std::ws >> DelimiterIO{'"'})
      {
        std::getline(ss, input.key3, '"');
        has_k3 = true;
      }
    }

    // После каждого поля ожидаем двоеточие ':'
    ss >> std::noskipws >> DelimiterIO{':'};
  }

  // Ожидаем закрывающую скобку ')'
  ss >> DelimiterIO{')'};

  if (ss && has_k1 && has_k2 && has_k3)
  {
    dest = input;
  }
  else
  {
    in.setstate(std::ios::failbit);
  }
  return in;
}

std::ostream &operator<<(std::ostream &out, const DataStruct &src)
{
  std::ostream::sentry sentry(out);
  if (!sentry)
    return out;

  // Выводим строго в формате из задания (без внутренних пробелов в ключах для стандартизации)
  out << "(:key1 #c(" << std::fixed << std::setprecision(1) << src.key1.real() << " " << src.key1.imag() << ")"
      << ":key2 " << std::scientific << std::setprecision(2) << src.key2
      << ":key3 \"" << src.key3 << "\":)";
  return out;
}
