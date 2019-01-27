#ifndef C4_RAW_TYPE_HPP
#define C4_RAW_TYPE_HPP

namespace ccc {

enum class RawTypeValue { VOID, CHAR, INT, POINTER, FUNCTION, STRUCT };

class RawType {
protected:
  RawType() = default;
  virtual ~RawType() = default;

public:
  virtual std::string print() = 0;
  virtual RawTypeValue getRawTypeValue() = 0;
  virtual std::shared_ptr<RawType> deref() { return nullptr; }
  virtual std::vector<std::shared_ptr<RawType>> get_param() { return {}; }
  virtual std::shared_ptr<RawType> get_return() { return nullptr; }
  virtual bool operator==(const std::shared_ptr<RawType> &) { return false; }
};

class RawScalarType : public RawType {
  RawTypeValue type_kind;

public:
  explicit RawScalarType(RawTypeValue v) : type_kind(v) {}

  std::string print() override {
    switch (type_kind) {
    case RawTypeValue::VOID:
      return "void";
    case RawTypeValue::INT:
      return "int";
    case RawTypeValue::CHAR:
      return "char";
    default:
      return "?";
    }
  }

  RawTypeValue getRawTypeValue() override { return type_kind; }

  bool operator==(const std::shared_ptr<RawType> &b) override {
    return getRawTypeValue() == b->getRawTypeValue();
  }
};

class RawPointerType : public RawType {
  std::shared_ptr<RawType> ptr;

public:
  explicit RawPointerType(std::shared_ptr<RawType> ptr) : ptr(std::move(ptr)) {}

  std::string print() override { return "&(" + ptr->print() + ")"; }

  RawTypeValue getRawTypeValue() override { return RawTypeValue::POINTER; }

  std::shared_ptr<RawType> deref() override { return ptr; }

  bool operator==(const std::shared_ptr<RawType> &b) override {
    if (b->getRawTypeValue() == RawTypeValue::POINTER)
      return ptr == b->deref();
    else
      return false;
  }
};

class RawFunctionType : public RawType {
  std::shared_ptr<RawType> ret_type;
  std::vector<std::shared_ptr<RawType>> param_types;

public:
  RawFunctionType(std::shared_ptr<RawType> ret_type,
                  std::vector<std::shared_ptr<RawType>> param_types)
      : ret_type(std::move(ret_type)), param_types(std::move(param_types)) {}

  std::string print() override {
    std::stringstream ss;
    for (const auto &t : param_types) {
      ss << t->print();
      if (t != param_types.back())
        ss << ",";
    }
    return "(" + ss.str() + ")->" + ret_type->print();
  }

  RawTypeValue getRawTypeValue() override { return RawTypeValue::FUNCTION; }

  std::vector<std::shared_ptr<RawType>> get_param() override {
    return param_types;
  }
  std::shared_ptr<RawType> get_return() override { return ret_type; }
};

class RawStructType : public RawType {
  std::string name;

public:
  explicit RawStructType(std::string name) : name(std::move(name)) {}

  std::string print() override { return "struct " + name; }

  RawTypeValue getRawTypeValue() override { return RawTypeValue::STRUCT; }
};

} // namespace ccc

#endif // C4_RAW_TYPE_HPP
