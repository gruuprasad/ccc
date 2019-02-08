#ifndef C4_RAW_TYPE_HPP
#define C4_RAW_TYPE_HPP

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Type.h"
#pragma GCC diagnostic pop

namespace ccc {

class GraphvizVisitor;
class PrettyPrinterVisitor;
class SemanticVisitor;
class CodegenVisitor;

// object type
enum class RawTypeValue { NIL, VOID, CHAR, INT, POINTER, FUNCTION, STRUCT };

class RawScalarType;
class RawPointerType;
class RawFunctionType;
class RawStructType;

// object structure for type representation used in semantical analysis instead
// of AST nodes
class RawType {
  FRIENDS

protected:
  RawType() = default;
  virtual ~RawType() = default;
  std::vector<int> elem_size = {};

public:
  virtual std::string print() = 0;

  // returns type of class as enum
  virtual RawTypeValue getRawTypeValue() = 0;

  // handle distinct members without casting
  virtual std::shared_ptr<RawType> deref() { return nullptr; }
  virtual std::vector<std::shared_ptr<RawType>> get_param() { return {}; }
  virtual std::shared_ptr<RawType> get_return() { return nullptr; }

  // decide if types are castable into each other
  virtual bool compare_equal(const std::shared_ptr<RawType> &) { return false; }

  // decide if types are the same
  virtual bool compare_exact(const std::shared_ptr<RawType> &) { return false; }

  // wrapper
  virtual RawScalarType *getRawScalarType() { return nullptr; }
  virtual RawPointerType *getRawPointerType() { return nullptr; }
  virtual RawFunctionType *getRawFunctionType() { return nullptr; }
  virtual RawStructType *getRawStructType() { return nullptr; }

  // called on pointer to decide
  virtual bool isVoidPtr() { return false; }

  // generate LLVM types
  virtual llvm::Type *getLLVMType(llvm::IRBuilder<>) { return nullptr; }
  virtual llvm::FunctionType *getLLVMFunctionType(llvm::IRBuilder<>) {
    return nullptr;
  }

  // used in codegen
  virtual int size() { return 8; };
  virtual void setSize(int){};
  virtual int ptr_size() { return size(); };
  virtual bool isFunctionPointer() { return false; };
};

class RawFunctionType : public RawType {
  FRIENDS
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

  llvm::FunctionType *getLLVMFunctionType(llvm::IRBuilder<> builder) override {
    std::vector<llvm::Type *> param;
    for (const auto &t : param_types)
      param.push_back(t->getLLVMType(builder));
    return llvm::FunctionType::get(ret_type->getLLVMType(builder), param,
                                   false);
  }
  int size() override { return 1; }
};

class RawScalarType : public RawType {
  FRIENDS
  RawTypeValue type_kind;
  int ptr_diff = -1;

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

  int size() override {
    if (ptr_diff > 0)
      return ptr_diff;
    switch (type_kind) {
    case RawTypeValue::INT:
      return 4;
    case RawTypeValue::CHAR:
      return 1;
    case RawTypeValue::NIL:
      return 4;
    default:
      return 1;
    }
  };

  void setSize(int s) override { ptr_diff = s; }

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
  llvm::Type *getLLVMType(llvm::IRBuilder<> builder) override {
    switch (type_kind) {
    case RawTypeValue::VOID:
      return builder.getInt8Ty();
    case RawTypeValue::INT:
      return builder.getInt32Ty();
    case RawTypeValue::CHAR:
      return builder.getInt8Ty();
    case RawTypeValue::NIL:
      return builder.getInt8Ty();
    default:
      return nullptr;
    }
  }
};

class RawPointerType : public RawType {
  FRIENDS
  std::shared_ptr<RawType> ptr;

public:
  explicit RawPointerType(std::shared_ptr<RawType> ptr) : ptr(std::move(ptr)) {}
  std::string print() override {
    if (ptr)
      return "&(" + ptr->print() + ")";
    return "&()";
  }
  RawTypeValue getRawTypeValue() override { return RawTypeValue::POINTER; }

  int size() override { return 8; }
  int ptr_size() override { return ptr->ptr_size(); }

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
      break;
    }
    return false;
  }
  bool compare_exact(const std::shared_ptr<RawType> &b) override {
    switch (b->getRawTypeValue()) {
    case RawTypeValue::POINTER:
      return ptr->compare_exact(b->deref());
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
  bool isFunctionPointer() override {
    if (ptr->getRawTypeValue() == RawTypeValue::FUNCTION)
      return true;
    else
      return ptr->isFunctionPointer();
  }

  llvm::Type *getLLVMType(llvm::IRBuilder<> builder) override {
    return llvm::PointerType::getUnqual(ptr->getLLVMType(builder));
  }
};

class RawStructType : public RawType {
  FRIENDS
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

  // calculate allignment of struct
  int size() override {
    int size = 0;
    if (!elem_size.empty()) {
      int pad = 1;
      for (int i : elem_size) {
        size += i;
        pad = std::max(pad, i);
        if (size > 0 && size % i != 0) {
          size += i - size % i;
        }
      }
      if (size > 0 && size % pad != 0) {
        size += pad - size % pad;
      }
    }
    return size;
  }
  RawStructType *getRawStructType() override { return this; }
  std::string getName() { return name; }
};

} // namespace ccc

#endif // C4_RAW_TYPE_HPP
