#ifndef C4_LOCATION_HPP
#define C4_LOCATION_HPP

#include <string>

namespace ccc {

class Location {
public:
  Location(unsigned long line, unsigned long column)
      : line(line), column(column) {}
  unsigned long getLine() const { return line; }
  unsigned long getColumn() const { return column + 1; }

  std::string getLocAsStr() {
    return std::to_string(getLine()) + ":" + std::to_string(getColumn());
  }

private:
  unsigned long line;
  unsigned long column;
};

} // namespace ccc

#endif
