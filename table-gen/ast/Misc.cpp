#include "Misc.h"
#include <string>

namespace ast::misc {
std::vector<llvm::Record*> collectDefsAndClasses(llvm::RecordKeeper& recordKeeper) {
  std::vector<llvm::Record*> records{};
  for (auto& record : recordKeeper.getClasses()) {
    records.push_back(record.second.get());
  }
  for (auto& record : recordKeeper.getDefs()) {
    records.push_back(record.second.get());
  }
  return records;
}

std::optional<ast::NodeKind> getKind(llvm::Record* record) {
  auto recordName = record->getName().str();
  if (recordName == "Terminal") {
    return std::make_optional(ast::NodeKind::Terminal);
  }
  else if (recordName == "NonTerminal") {
    return std::make_optional(ast::NodeKind::NonTerminal);
  }
  else if (record->getType()->getClasses().empty()) {
    return std::optional<ast::NodeKind>{};
  }

  for (auto* parent : record->getType()->getClasses()) {
    if (auto subResult = getKind(parent)) {
      return subResult;
    }
  }

  return std::optional<ast::NodeKind>{};
}


std::optional<std::string> getInitValue(const llvm::RecordVal* recordVal) {
  auto* value = recordVal->getValue();
  if (!dynamic_cast<llvm::UnsetInit*>(value)) {
    return std::make_optional(value->getAsString());
  }
  return std::optional<std::string>{};
}


std::unique_ptr<ast::Type> buildType(llvm::RecTy* recordType) {
  switch (recordType->getRecTyKind()) {
    case llvm::RecTy::RecTyKind::BitRecTyKind : {
      return std::make_unique<ast::BoolType>();
    }
    case llvm::RecTy::RecTyKind::IntRecTyKind : {
      return std::make_unique<ast::IntType>();
    }
    case llvm::RecTy::RecTyKind::StringRecTyKind : {
      return std::make_unique<ast::StringType>();
    }
    case llvm::RecTy::RecTyKind::RecordRecTyKind : {
      return std::make_unique<ast::PtrType>(recordType->getAsString());
    }
    case llvm::RecTy::RecTyKind::ListRecTyKind : {
      auto* listType = dynamic_cast<llvm::ListRecTy*>(recordType);
      auto elementType = buildType(listType->getElementType());
      return std::make_unique<ast::Composite>(std::move(elementType));
    }
    default : {
      return nullptr;
    }
  }
}


std::vector<ast::Attribute> getAttributes(llvm::ArrayRef<llvm::RecordVal> recordValues) {
  std::vector<ast::Attribute> attributes{};
  for(auto& recordValue : recordValues) {
    auto name = recordValue.getName().str();
    auto type = ast::misc::buildType(recordValue.getType());
    auto initValue = getInitValue(&recordValue);
    attributes.push_back(Attribute{name, std::move(type), initValue});
  }
  return attributes;
}


std::string capitalize(std::string str) {
  str[0] = std::toupper(str[0]);
  return str;
}
} // namespace ast::misc