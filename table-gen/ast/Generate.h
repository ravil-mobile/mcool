#pragma once

#include "Misc.h"

namespace ast {
void genConstructor(llvm::raw_ostream&, const std::string&, const std::vector<ast::Attribute>&);
void genGetters(llvm::raw_ostream&, const std::vector<ast::Attribute>&);
void genSetters(llvm::raw_ostream&, const std::vector<ast::Attribute>&);
} // namespace ast