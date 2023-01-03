#include "Generate.h"

namespace ast {
void genConstructor(llvm::raw_ostream& OS,
                    const std::string& className,
                    const std::vector<ast::Attribute>& attributes) {

  std::vector<const ast::Attribute*> builtinAttributes{};
  for (auto& attr : attributes) {
    if (attr.type->isBuiltin() || attr.type->isPtr()) {
      builtinAttributes.push_back(&attr);
    }
  }

  if (builtinAttributes.empty()) {
    OS << "  " << className << "() {}\n";
  }
  else {
    const size_t numAttributes{builtinAttributes.size()};
    auto getDelimiter = [numAttributes](size_t i){
      const auto isTheLast = (i == (numAttributes - 1));
      return isTheLast ? "" : ", ";
    };

    OS << "  explicit " << className << "(";
    for (size_t i = 0; i < builtinAttributes.size(); ++i) {
      std::string sep = getDelimiter(i);
      OS << attributes[i].type->getName() << " " << attributes[i].name << sep;
    }
    OS << ") : ";

    for (size_t i = 0; i < builtinAttributes.size(); ++i) {
      std::string sep = getDelimiter(i);
      OS << attributes[i].name << "(" << attributes[i].name << ")" << sep;
    }

    OS << " {}\n";
  }
}


void genGetters(llvm::raw_ostream& OS,
                const std::vector<ast::Attribute>& attributes) {
  for (auto& attr : attributes) {
    OS << "  " << attr.type->getName() << " get" << ast::misc::capitalize(attr.name);
    OS << "() { return " << attr.name << "; }\n";
  }
}


void genSetters(llvm::raw_ostream& OS,
                const std::vector<ast::Attribute>& attributes) {
  for (auto& attr: attributes) {
    if (attr.type->isComposite()) {
      auto* compositeType = dynamic_cast<ast::Composite*>(attr.type.get());
      auto* elementType = compositeType->getInnerType();
      OS << "  void add(" << elementType->getName() << " item) { " << attr.name << ".push_back(item); }";
    }
  }
}

} // namespace ast