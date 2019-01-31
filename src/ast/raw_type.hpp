#ifndef C4_RAW_TYPE_HPP
#define C4_RAW_TYPE_HPP

namespace ccc {

enum class RawTypeValue { NIL, VOID, CHAR, INT, POINTER, FUNCTION, STRUCT };

class RawScalarType;
class RawPointerType;
class RawFunctionType;
class RawStructType;

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
  virtual bool compare_equal(const std::shared_ptr<RawType> &) { return false; }
  virtual bool compare_exact(const std::shared_ptr<RawType> &) { return false; }
  virtual RawScalarType *getRawScalarType() { return nullptr; }
  virtual RawPointerType *getRawPointerType() { return nullptr; }
  virtual RawFunctionType *getRawFunctionType() { return nullptr; }
  virtual RawStructType *getRawStructType() { return nullptr; }
  virtual bool isVoidPtr() { return false; }
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
        ss << ", ";
    }
    return "(" + ss.str() + ")->" + ret_type->print();
  }
  RawTypeValue getRawTypeValue() override { return RawTypeValue::FUNCTION; }
  std::vector<std::shared_ptr<RawType>> get_param() override {
    return param_types;
  }
  std::shared_ptr<RawType> get_return() override { return ret_type; }
  bool compare_equal(const std::shared_ptr<RawType> &b) override {
    if (b == nullptr)
      return false;
    switch (b->getRawTypeValue()) {
    case RawTypeValue::FUNCTION: {
      auto tmp = b->getRawFunctionType()->param_types;
      if (param_types.size() != tmp.size())
        return false;
      for (size_t i = 0; i < param_types.size(); i++)
        if (!param_types[i]->compare_equal(tmp[i]))
          return false;
      return ret_type->compare_equal(b->getRawFunctionType()->ret_type);
    }
    case RawTypeValue::INT:
    case RawTypeValue::CHAR:
      return b->compare_equal(ret_type);
    case RawTypeValue::POINTER:
      return b->isVoidPtr() || b->deref()->compare_equal(ret_type);
    case RawTypeValue::NIL:
      return true;
    default:
      return false;
    }
  }
  bool compare_exact(const std::shared_ptr<RawType> &b) override {
    if (b == nullptr)
      return false;
    switch (b->getRawTypeValue()) {
    case RawTypeValue::FUNCTION: {
      auto tmp = b->getRawFunctionType()->param_types;
      if (param_types.size() != tmp.size())
        return false;
      for (size_t i = 0; i < param_types.size(); i++)
        if (!param_types[i]->compare_exact(tmp[i]))
          return false;
      return ret_type->compare_exact(b->getRawFunctionType()->ret_type);
    }
    case RawTypeValue::NIL:
      return true;
    default:
      return false;
    }
  }
  RawFunctionType *getRawFunctionType() override { return this; }
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
    case RawTypeValue::NIL:
      return "null";
    default:
      return "?";
    }
  }
  RawTypeValue getRawTypeValue() override { return type_kind; }
  bool compare_equal(const std::shared_ptr<RawType> &b) override {
    if (b == nullptr)
      return false;
    switch (b->getRawTypeValue()) {
    case RawTypeValue::VOID:
      return type_kind == RawTypeValue::VOID;
    case RawTypeValue::NIL:
    case RawTypeValue::INT:
    case RawTypeValue::CHAR:
      return type_kind == RawTypeValue::INT ||
             type_kind == RawTypeValue::CHAR || type_kind == RawTypeValue::NIL;
    case RawTypeValue::POINTER:
      if (type_kind == RawTypeValue::VOID)
        return false;
      return compare_equal(std::make_shared<RawScalarType>(RawTypeValue::INT));
    case RawTypeValue::FUNCTION:
      return compare_equal(b->getRawFunctionType()->get_return());
    default:
      return false;
    }
  }
  bool compare_exact(const std::shared_ptr<RawType> &b) override {
    if (b == nullptr)
      return false;
    switch (b->getRawTypeValue()) {
    case RawTypeValue::VOID:
      return type_kind == RawTypeValue::VOID;
    case RawTypeValue::NIL:
    case RawTypeValue::INT:
      return type_kind == RawTypeValue::INT || type_kind == RawTypeValue::NIL;
    case RawTypeValue::CHAR:
      return type_kind == RawTypeValue::CHAR || type_kind == RawTypeValue::NIL;
    default:
      return false;
    }
  }
  RawScalarType *getRawScalarType() override { return this; }
};

class RawPointerType : public RawType {
  std::shared_ptr<RawType> ptr;

public:
  explicit RawPointerType(std::shared_ptr<RawType> ptr) : ptr(std::move(ptr)) {}
  std::string print() override {
    if (ptr)
      return "&(" + ptr->print() + ")";
    return "&()";
  }
  RawTypeValue getRawTypeValue() override { return RawTypeValue::POINTER; }
  std::shared_ptr<RawType> deref() override { return ptr; }
  bool compare_equal(const std::shared_ptr<RawType> &b) override {
    if (b == nullptr)
      return false;
    switch (b->getRawTypeValue()) {
    case RawTypeValue::POINTER:
      if (isVoidPtr() || b->isVoidPtr())
        return true;
      if (ptr)
        return ptr->compare_equal(b->deref());
      break;
    case RawTypeValue::NIL:
    case RawTypeValue::CHAR:
    case RawTypeValue::INT:
      return true;
    case RawTypeValue::FUNCTION:
      if (ptr)
        return ptr->compare_equal(b->get_return());
    default:
      return false;
    }
  }
  bool compare_exact(const std::shared_ptr<RawType> &b) override {
    switch (b->getRawTypeValue()) {
    case RawTypeValue::POINTER:
      return ptr->compare_equal(b->deref());
    case RawTypeValue::NIL:
      return true;
    default:
      return false;
    }
  }
  bool isVoidPtr() override {
    if (ptr)
      return ptr->getRawTypeValue() == RawTypeValue::VOID || ptr->isVoidPtr();
    else
      return false;
  }
  std::shared_ptr<RawType> get_return() override { return ptr->get_return(); }
};

class RawStructType : public RawType {
  std::string name;

public:
  explicit RawStructType(std::string name) : name(std::move(name)) {}
  std::string print() override { return name; }
  RawTypeValue getRawTypeValue() override { return RawTypeValue::STRUCT; }
  bool compare_equal(const std::shared_ptr<RawType> &b) override {
    if (b == nullptr)
      return false;
    switch (b->getRawTypeValue()) {
    case RawTypeValue::STRUCT:
      return name == b->getRawStructType()->name ||
             name + "." == b->getRawStructType()->name ||
             name == b->getRawStructType()->name + ".";
    default:
      return false;
    }
  }
  bool compare_exact(const std::shared_ptr<RawType> &b) override {
    return compare_equal(b);
  }
  RawStructType *getRawStructType() override { return this; }
  std::string getName() { return name; }
};

} // namespace ccc

#endif // C4_RAW_TYPE_HPP
