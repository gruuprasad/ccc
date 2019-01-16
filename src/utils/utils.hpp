#ifndef UTILS_HPP
#define UTILS_HPP

#include "../ast/ast_node.hpp"
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <sstream>
#include <type_traits>
#include <utility>

namespace ccc {

// XXX with C++14 wouldn't we have to call
// std::make_unique instead of ccc::make_unique?
//#if __cplusplus == 201103L
// Reference:
// https://stackoverflow.com/questions/17902405/how-to-implement-make-unique-function-in-c11
// Helper method to create unique_ptr instance, which is available only in
// compilers with c++14 support.
template <class T> struct _Unique_if {
  typedef std::unique_ptr<T> _Single_object;
};

template <class T> struct _Unique_if<T[]> {
  typedef std::unique_ptr<T[]> _Unknown_bound;
};

template <class T, size_t N> struct _Unique_if<T[N]> {
  typedef void _Known_bound;
};

template <class T, class... Args>
typename _Unique_if<T>::_Single_object make_unique(Args &&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <class T>
typename _Unique_if<T>::_Unknown_bound make_unique(size_t n) {
  typedef typename std::remove_extent<T>::type U;
  return std::unique_ptr<T>(new U[n]());
}

template <class T, class... Args>
typename _Unique_if<T>::_Known_bound make_unique(Args &&...) = delete;

//#endif

struct EnumClassHash {
  template <typename T> std::size_t operator()(T t) const {
    return static_cast<std::size_t>(t);
  }
};

class Utils {
public:
  static std::vector<std::string> split_lines(const std::string &str) {
    std::stringstream ss(str);
    std::string tmp;
    std::vector<std::string> split = std::vector<std::string>();
    while (std::getline(ss, tmp, '\n')) {
      while (tmp[tmp.length() - 1] == '\r' || tmp[tmp.length() - 1] == '\n') {
        tmp.pop_back();
      }
      split.push_back(tmp);
    }
    return split;
  }

  static bool compare(const std::string &content, const std::string &expected) {
    if (expected != content) {
      std::vector<std::string> expected_lines = split_lines(expected);
      std::vector<std::string> content_lines = split_lines(content);
      for (unsigned long i = 0, errors = 0;
           i < std::max(content_lines.size(), expected_lines.size()); i++) {
        if (i >= expected_lines.size()) {
          std::cerr << "expected nothing but got \"" << content_lines[i] << "\""
                    << std::endl;
        } else if (i >= content_lines.size()) {
          std::cerr << "expected \"" << expected_lines[i]
                    << "\" but got nothing" << std::endl;
        } else if (content_lines[i] != expected_lines[i]) {
          std::cerr << "expected \"" << expected_lines[i] << "\" but got \""
                    << content_lines[i] << "\"" << std::endl;
        } else {
          continue;
        }
        errors++;
        if (errors > 5) {
          std::cerr << std::endl
                    << "Output truncated after five errors... - fix your stuff!"
                    << std::endl;
          break;
        }
      }
      std::cout << content << std::endl;
      return false;
    }
    return true;
  }

  template <class ListType, typename... Args>
  static ListType vector(Args... args) {
    return vector(ListType(), std::move(args)...);
  }
  template <class ListType, typename... Args>
  static ListType vector(ListType block, Args... args) = delete;
  template <class ListType, class T, typename... Args>
  static ListType vector(ListType block, T t, Args... args) {
    block.push_back(std::move(t));
    return vector(std::move(block), std::move(args)...);
  }
  template <class ListType> static ListType vector(ListType block) {
    return block;
  }
};

// Read translation unit from .c4 file.
// Format of the code in .c4
// (startunit + empty line + input code + + emoty line endunit)+

class Reader {
public:
  Reader(std::ifstream &in_) : in(in_) {}

  std::string readLine() {
    cur_line++;
    // State variables
    std::string line;
    if (getline(in, line).good()) {
      return line;
    }

    if (in.eof()) {
      stopped = true;
      return std::string();
    }

    if (!in) {
      std::cerr << "Error in line " << cur_line << std::endl;
      exit(-1);
    }

    return line;
  }

  std::string readUnit() {
    // Go to start of code
    while (true) {
      if (readLine() == "startunit") {
        break;
      }
    }

    // Read code
    std::string unit;
    std::string line;
    while (true) {
      line = readLine();
      if (stopped) {
        return unit;
      }
      if (line == "endunit") {
        break;
      }
      unit.append(line);
    }
    return unit;
  }

  bool is_eof() { return stopped; }

private:
  int cur_line = 0;
  bool stopped = false;
  std::ifstream &in;
};

} // namespace ccc
#endif