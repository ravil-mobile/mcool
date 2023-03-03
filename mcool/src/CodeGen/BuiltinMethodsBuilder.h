#pragma once

#include "CodeGen/BaseBuilder.h"

namespace mcool::codegen {
class BuiltinMethodsBuilder : public BaseBuilder {
public:
  explicit BuiltinMethodsBuilder(Environment& env) : BaseBuilder(env) {}
  void build();
private:
  void genPrintfDecl();
  void genMallocDecl();
  void genFreeDecl();
  void genExitDecl();
  void genObjectCopy();
  void genIOOutString();
  void genIOOutInt();
};

} // namespace mcool::codegen