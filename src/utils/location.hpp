#ifndef C4_LOCATION_HPP
#define C4_LOCATION_HPP

#include <ostream>
#include <string>

namespace ccc {

class Location {
public:
  Location(unsigned long line, unsigned long column)
      : line(line), column(column) {}
  unsigned long getLine() const { return line; }
  unsigned long getColumn() const { return column + 1; }

  friend std::ostream &operator<<(std::ostream &os, const Location &loc) {
    os << loc.getLine() << ":" << loc.getColumn();
    return os;
  }

private:
  unsigned long line;
  unsigned long column;
};

} // namespace ccc

#endif
