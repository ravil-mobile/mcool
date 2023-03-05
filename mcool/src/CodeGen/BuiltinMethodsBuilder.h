#pragma once

#include "CodeGen/BaseBuilder.h"

namespace mcool::codegen {
class BuiltinMethodsBuilder : public BaseBuilder {
  public:
  explicit BuiltinMethodsBuilder(Environment& env) : BaseBuilder(env) {}
  void build();

  private:
  void genCStdFunctions();
  void genClearStdinBuffer();
  void genObjectCopy();
  void genObjectAbort();
  void genObjectTypeName();
  void genIOOutString();
  void genIOOutInt();
  void genIOInInt();
  void genIOInString();
  void genStringLength();
};

} // namespace mcool::codegen