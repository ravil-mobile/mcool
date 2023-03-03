#include "Types/Types.h"
#include <cstddef>
#include <cassert>

namespace mcool::type {
bool MethodType::isSame(Type* other) {
  assert(other != nullptr);
  if (auto* otherType = dynamic_cast<MethodType*>(other)) {
    if (this->methodName != otherType->methodName) {
      return false;
    }

    const size_t size{parameters.size()};
    if (size != otherType->parameters.size()) {
      return false;
    }

    for (size_t i = 0; i < size; ++i) {
      if (parameters[i]->getAsString() != otherType->parameters[i]->getAsString()) {
        return false;
      }
    }

    return true;
  } else {
    return false;
  }
}
} // namespace mcool::type