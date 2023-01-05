#include "GeneratorAux.h"
#include "StreamMisc.h"
#include <sstream>

namespace ast {
std::string genConstructor(const std::string& className, std::vector<inheritance::tree::Node*>& chain) {

  std::stringstream stream;

  auto parameterList = genConstructorParameterList(chain);
  auto initList = genConstructorInitList(chain);

  stream << "  " << className << "(" << parameterList << ")";
  if (not initList.empty()) {
    stream << " : " << initList;
  }
  stream << " {}\n";
  return stream.str();
}


std::string genConstructorParameterList(std::vector<inheritance::tree::Node*>& chain) {
  std::stringstream stream;
  bool cleanTail{false};
  for (auto* node : chain) {
    auto attrs = node->getAttributes();
    for (auto* attr : attrs) {
      if (attr->type->isBuiltin() || attr->type->isPtr()) {
        stream << attr->type->getName() << " " << attr->name << ", ";
        cleanTail = true;
      }
    }
  }

  mcool::removeFromBack(stream, cleanTail ? 2 : 0);
  return stream.str();
}

std::string genConstructorInitList(std::vector<inheritance::tree::Node*>& chain) {
  std::stringstream stream;
  bool cleanTail{false};

  if (not chain.empty()) {
    auto *currNode = chain[0];
    auto currAttrs = currNode->getAttributes();
    for (auto *attr: currAttrs) {
      if (attr->type->isBuiltin() || attr->type->isPtr()) {
        stream << attr->name << "(" << attr->name << "), ";
        cleanTail = true;
      }
    }

    for (size_t i{1}; i < chain.size(); ++i) {
      auto *node = chain[i];
      auto attrs = node->getAttributes();
      if (not attrs.empty()) {
        stream << genConstructorCall(node->getName(), attrs);
        stream << ", ";
        cleanTail = true;
      }
    }

  }
  mcool::removeFromBack(stream, cleanTail ? 2 : 0);
  return stream.str();
}

std::string genConstructorCall(const std::string& className,
                               std::vector<Attribute*>& attrs) {
  std::stringstream stream;
  bool cleanTail{false};

  stream << className << "(";
  for (auto *attr: attrs) {

    if (attr->type->isBuiltin() || attr->type->isPtr()) {
      stream << attr->name << ", ";
      cleanTail = true;
    }
  }
  mcool::removeFromBack(stream, cleanTail ? 2 : 0);

  stream << ")";
  return stream.str();
}


void genGetters(llvm::raw_ostream& OS, const std::vector<ast::Attribute*>& attributes) {
  for (auto& attr : attributes) {
    auto typeName = attr->type->getName();
    bool useReference = attr->type->isComposite();
    if (useReference) {
      typeName += '&';
    }

    OS << "  " << typeName << " get" << ast::misc::capitalize(attr->name);
    OS << "() { return " << attr->name << "; }\n";
  }
}

void genSetters(llvm::raw_ostream& OS, const std::vector<ast::Attribute*>& attributes) {
  for (auto& attr: attributes) {
    if (attr->type->isComposite()) {
      auto* compositeType = dynamic_cast<ast::Composite*>(attr->type.get());
      auto* elementType = compositeType->getInnerType();
      OS << "  void add(" << elementType->getName() << " item) { " << attr->name << ".push_back(item); }";
    }
  }
}

} // namespace ast