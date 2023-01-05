#pragma once

#include "Misc.h"
#include "InheritanceTree.h"

namespace ast {
std::string genConstructor(const std::string& className, std::vector<inheritance::tree::Node*>& chain);
std::string genConstructorParameterList(std::vector<inheritance::tree::Node*>& chain);
std::string genConstructorInitList(std::vector<inheritance::tree::Node*>& chain);
std::string genConstructorCall(const std::string& className, std::vector<Attribute*>& attrs);

void genGetters(llvm::raw_ostream&, const std::vector<ast::Attribute*>&);
void genSetters(llvm::raw_ostream&, const std::vector<ast::Attribute*>&);
} // namespace ast