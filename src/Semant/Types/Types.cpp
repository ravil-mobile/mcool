#include "Semant/Types/Types.h"
#include <cstddef>
#include <cassert>

namespace mcool::semant {
bool MethodType::isSame(MethodType* type) {
  assert(type != nullptr);

  if (this->methodName != type->methodName) {
    return false;
  }

  const size_t size{parameters.size()};
  if (size != type->parameters.size()) {
    return false;
  }

  for (size_t i = 0; i < size; ++i) {
    if (parameters[i]->getAsString() != type->parameters[i]->getAsString()) {
      return false;
    }
  }

  return true;
}
} // namespace mcool::semant