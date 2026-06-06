#include "DataStruct.h"
#include "iofmtguard.hpp"
#include <limits>
#include <iomanip>

// ============ ВСПОМОГАТЕЛЬНЫЕ ОПЕРАТОРЫ ВВОДА ============

std::istream& operator>>(std::istream& in, DelimiterIO&& dest)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }
  char c = '0';
  in >> c;
  if (in && (c != dest.exp))
  {
    in.setstate(std::ios::failbit);
  }
  return in;
}

std::istream& operator>>(std::istream& in, UllHexIO&& dest)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }
  in >> std::hex >> dest.ref >> std::dec;
  return in;
}

std::istream& operator>>(std::istream& in, CharLitIO&& dest)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }
  in >> DelimiterIO{ '\'' };
  in.get(dest.ref);
  in >> DelimiterIO{ '\'' };
  return in;
}

std::istream& operator>>(std::istream& in, StringIO&& dest)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }
  return std::getline(in >> DelimiterIO{ '"' }, dest.ref, '"');
}

std::istream& operator>>(std::istream& in, LabelIO&& dest)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }
  std::string data;
  if ((in >> data) && (data != dest.exp))
  {
    in.setstate(std::ios::failbit);
  }
  return in;
}

// ============ ОПЕРАТОР ВВОДА ДЛЯ DataStruct ============

std::istream& operator>>(std::istream& in, DataStruct& dest)
{
  while (in.good())
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      break;
    }

    DataStruct input;
    bool hasKey1 = false;
    bool hasKey2 = false;
    bool hasKey3 = false;
    std::streampos lineStart = in.tellg();

    {
      using sep = DelimiterIO;
      using hex = UllHexIO;
      using chr = CharLitIO;
      using str = StringIO;

      in >> sep{ '(' } >> sep{ ':' };

      for (int i = 0; i < 3 && in; ++i)
      {
        std::string keyName;
        in >> keyName;

        if (!in)
        {
          break;
        }

        if (keyName == "key1")
        {
          in >> hex{ input.key1 } >> sep{ ':' };
          if (in)
          {
            hasKey1 = true;
          }
        }
        else if (keyName == "key2")
        {
          in >> chr{ input.key2 } >> sep{ ':' };
          if (in)
          {
            hasKey2 = true;
          }
        }
        else if (keyName == "key3")
        {
          in >> str{ input.key3 } >> sep{ ':' };
          if (in)
          {
            hasKey3 = true;
          }
        }
        else
        {
          in.setstate(std::ios::failbit);
        }
      }

      if (in)
      {
        in >> sep{ ')' };
      }
    }

    if (in && hasKey1 && hasKey2 && hasKey3)
    {
      dest = std::move(input);
      return in;
    }

    in.clear();
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }

  in.setstate(std::ios::failbit);
  return in;
}

// ============ ОПЕРАТОР ВЫВОДА ДЛЯ DataStruct ============

std::ostream& operator<<(std::ostream& out, const DataStruct& src)
{
  std::ostream::sentry sentry(out);
  if (!sentry)
  {
    return out;
  }
  iofmtguard fmtguard(out);
  out << "(:key1 0x" << std::hex << std::uppercase << src.key1;
  out << ":key2 '" << src.key2 << "'";
  out << ":key3 \"" << src.key3 << "\"";
  out << ":)";
  return out;
}

// ============ ОПЕРАТОР СРАВНЕНИЯ ============

bool DataStruct::operator<(const DataStruct& other) const
{
  if (key1 != other.key1)
  {
    return key1 < other.key1;
  }
  if (key2 != other.key2)
  {
    return key2 < other.key2;
  }
  return key3.size() < other.key3.size();
}

// ============ КОМПАРАТОР ============

bool DataStructComparator::operator()(
    const DataStruct& lhs,
    const DataStruct& rhs) const
{
  return lhs < rhs;
}
