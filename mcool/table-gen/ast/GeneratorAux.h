#pragma once

#include "Misc.h"
#include "InheritanceTree.h"
#include <ostream>

namespace ast {
std::string genConstructor(const std::string& className, std::vector<inheritance::tree::Node*>& chain);
std::string genConstructorParameterList(std::vector<inheritance::tree::Node*>& chain);
std::string genConstructorInitList(std::vector<inheritance::tree::Node*>& chain);
std::string genConstructorCall(const std::string& className, std::vector<Attribute*>& attrs);
std::string genParentConstructorCall(std::vector<inheritance::tree::Node*>& chain);

void genGetters(std::ostream&, const std::vector<ast::Attribute*>&);
void genSetters(std::ostream&, const std::vector<ast::Attribute*>&);
} // namespace ast