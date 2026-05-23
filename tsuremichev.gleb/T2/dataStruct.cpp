#include "dataStruct.hpp"
#include <iomanip>

// Чтение конкретного символа-разделителя
std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
  std::istream::sentry sentry(in);
  if (!sentry) return in;
  char c;
  in >> c;
  if (in && c != dest.expected) {
    in.setstate(std::ios::failbit);
  }
  return in;
}

// Чтение метки ключа (например, ":key1")
std::istream& operator>>(std::istream& in, LabelIO&& dest) {
  std::istream::sentry sentry(in);
  if (!sentry) return in;
  std::string actual;
  for (size_t i = 0; i < dest.expected.length(); ++i) {
    char c;
    in >> c;
    actual += c;
  }
  if (actual != dest.expected) {
    in.setstate(std::ios::failbit);
  }
  return in;
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
  std::istream::sentry sentry(in);
  if (!sentry) return in;

  DataStruct input;
  in >> DelimiterIO{'('} >> DelimiterIO{':'};

  for (int i = 0; i < 3; ++i) {
    std::string key;
    std::getline(in, key, ' '); // Читаем имя ключа до пробела ("key1", "key2" или "key3")

    if (key == "key1") {
      // Читаем double в научном формате (например, 5.45e-2)
      in >> input.key1;
    } else if (key == "key2") {
      // Читаем комплексное число #c(1.0 -1.0)
      double re, im;
      in >> LabelIO{"#c("} >> re >> im >> DelimiterIO{')'};
      input.key2 = {re, im};
    } else if (key == "key3") {
      // Читаем строку в кавычках
      in >> DelimiterIO{'"'};
      std::getline(in, input.key3, '"');
    }

    if (i < 2) in >> DelimiterIO{':'}; // Между полями должен быть двоеточие
  }

  in >> DelimiterIO{':'} >> DelimiterIO{')'};

  if (in) {
    dest = input;
  }
  return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
  std::ostream::sentry sentry(out);
  if (!sentry) return out;

  out << "(:key1 " << std::scientific << src.key1
  << ":key2 #c(" << std::fixed << std::setprecision(1) << src.key2.real() << " " << src.key2.imag() << ")"
  << ":key3 \"" << src.key3 << "\":)";
  return out;
}
