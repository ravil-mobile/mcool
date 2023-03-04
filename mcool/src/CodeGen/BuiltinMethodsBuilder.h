#pragma once

#include "CodeGen/BaseBuilder.h"

namespace mcool::codegen {
class BuiltinMethodsBuilder : public BaseBuilder {
public:
  explicit BuiltinMethodsBuilder(Environment& env) : BaseBuilder(env) {}
  void build();
private:
  void genCStdFunctions();
  void genObjectCopy();
  void genIOOutString();
  void genIOOutInt();
  void genIOInInt();
};

} // namespace mcool::codegen