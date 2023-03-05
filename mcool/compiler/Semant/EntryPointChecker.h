#pragma once

#include "ast.h"
#include "Types/Types.h"
#include "Semant/TypeChecker/Error.h"
#include <sstream>

namespace mcool::semant {
class EntryPointChecker : public Error {
  public:
  void run(ast::CoolClassList* coolClassList);

  private:
  bool assertSingleMainClass(ast::CoolClassList* coolClassList);
  bool assertSingleMainMethod();
  bool assertCorrectMainMethodSignature();

  ast::CoolClass* mainClass{};
  ast::SingleMethod* mainMethod{};
  std::stringstream errStream;
};
} // namespace mcool::semant