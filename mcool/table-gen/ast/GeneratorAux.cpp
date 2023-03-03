#include "GeneratorAux.h"
#include "StreamMisc.h"

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

    auto parentConstructor = genParentConstructorCall(chain);
    if (not parentConstructor.empty()) {
      stream << parentConstructor;
      cleanTail = false;
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

std::string genParentConstructorCall(std::vector<inheritance::tree::Node*>& chain) {
  std::stringstream stream;
  bool cleanTail{false};

  if (chain.size() > 1) {
    std::string parentClass{chain[1]->getName()};
    stream << chain[1]->getName() << "(";

    for (size_t i{1}; i < chain.size(); ++i) {
      auto *node = chain[i];
      auto attrs = node->getAttributes();

      if (not attrs.empty()) {
        for (auto *attr: attrs) {
          if (attr->type->isBuiltin() || attr->type->isPtr()) {
            stream << attr->name << ", ";
            cleanTail = true;
          }
        }
      }
    }
    mcool::removeFromBack(stream, cleanTail ? 2 : 0);

    stream << ")";
  }

  return stream.str();
}


void genGetters(std::ostream& OS, const std::vector<ast::Attribute*>& attributes) {
  for (auto& attr : attributes) {
    auto typeName = attr->type->getName();

    if (typeName == "StringPtr*") {
      OS << "  std::string&" << " get" << ast::misc::capitalize(attr->name) << "AsStr";
      OS << "() { return " << attr->name << "->get(); }\n";

      OS << "  const std::string&" << " get" << ast::misc::capitalize(attr->name) << "AsStr";
      OS << "() const { return " << attr->name << "->get(); }\n";
    }
    else {
      if (attr->type->preferReference()) {
        typeName += '&';
      }

      OS << "  " << typeName << " get" << ast::misc::capitalize(attr->name);
      OS << "() { return " << attr->name << "; }\n";
    }
  }
}

void genSetters(std::ostream& OS, const std::vector<ast::Attribute*>& attributes) {
  for (auto& attr: attributes) {
    auto *type = attr->type.get();
    if (auto* compositeType = dynamic_cast<ast::Composite*>(type)) {
      auto* elementType = compositeType->getInnerType();
      OS << "  void add(" << elementType->getName() << " item) { " << attr->name << ".push_back(item); }";
    }
    if (auto* locationType = dynamic_cast<ast::LocationType*>(type)) {
      OS << "  void setLocation(" << locationType->getName() << "& loc) { location = loc; }";
    }
  }
}

} // namespace ast