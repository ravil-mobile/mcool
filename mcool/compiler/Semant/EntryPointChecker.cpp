#include "Semant/EntryPointChecker.h"

namespace mcool::semant {
void EntryPointChecker::run(ast::CoolClassList* coolClassList) {
  auto isOK = assertSingleMainClass(coolClassList);
  if (not isOK) {
    return;
  }

  isOK = assertSingleMainMethod();
  if (not isOK) {
    return;
  }

  assertCorrectMainMethodSignature();
}

bool EntryPointChecker::assertSingleMainClass(ast::CoolClassList* coolClassList) {
  std::vector<mcool::ast::CoolClass*> mainCoolClasses{};
  for (auto* coolClass : coolClassList->getData()) {
    auto& coolClassName = coolClass->getCoolType()->getNameAsStr();
    if (coolClassName == "Main") {
      mainCoolClasses.push_back(coolClass);
    }
  }

  if (mainCoolClasses.empty()) {
    errList.emplace_back("No `Main` class found");
    return false;
  }

  if (mainCoolClasses.size() != 1) {
    errStream << "More than one `Main` class found. See: \n";
    for (auto* mainCoolClass : mainCoolClasses) {
      errStream << '\t' << mainCoolClass->getLocation();
    }
    errList.push_back(errStream.str());
    return false;
  }

  mainClass = mainCoolClasses[0];
  return true;
}

bool EntryPointChecker::assertSingleMainMethod() {
  std::vector<mcool::ast::SingleMethod*> mainMethods{};
  for (auto* attr : mainClass->getAttributes()->getData()) {
    if (auto* method = dynamic_cast<mcool::ast::SingleMethod*>(attr)) {
      auto& methodName = method->getId()->getNameAsStr();
      if (methodName == "main") {
        mainMethods.push_back(method);
      }
    }
  }

  if (mainMethods.empty()) {
    errStream << "No `main` method in `Main` class. See: ";
    errStream << mainClass->getLocation();
    errList.emplace_back(errStream.str());
    return false;
  }

  if (mainMethods.size() != 1) {
    errStream << "More than one `main` method found in `Main` class. See: \n";
    for (auto* method : mainMethods) {
      errStream << '\t' << method->getLocation();
    }
    errList.emplace_back(errStream.str());
    return false;
  }

  mainMethod = mainMethods[0];
  return true;
}

bool EntryPointChecker::assertCorrectMainMethodSignature() {
  auto returnTypeName = mainMethod->getReturnType()->getNameAsStr();
  if (returnTypeName != "Object") {
    errStream << "Expected `Object` return type from the `main` method. "
              << "Given: `" << returnTypeName << "`. See " << mainMethod->getLocation();
    errList.emplace_back(errStream.str());
    return false;
  }

  auto& params = mainMethod->getParameters()->getFormals();
  if (not params.empty()) {
    errStream << "Expected no parameters in the `main` method. See "
              << mainMethod->getParameters()->getLocation();
    errList.emplace_back(errStream.str());
    return false;
  }

  return true;
}
} // namespace mcool::semant