#pragma once

#include "ast.h"
#include <unordered_map>

namespace mcool::semant {
using ClassTableType = std::unordered_map<std::string, mcool::ast::CoolClass*>;
} // namespace mcool::semant