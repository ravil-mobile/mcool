#pragma once

#include "Parser/Scanner.h"
#include "Parser.h"
#include "Context.h"
#include "ast.h"
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <cassert>
#include <utility>
#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"

namespace mcool::tests::parser {
class TestDriver {
  public:
  explicit TestDriver(std::istream& stream) : stream(stream) {}

  auto run() {
    mcool::AstTree astTree{};

    mcool::Scanner scanner(true);
    mcool::Parser parser(scanner, astTree, context.getMemoryManagement());

    scanner.set(&stream, &inputFileName);
    bool parserStatus = parser.parse() == 0;

    return std::make_tuple(parserStatus, astTree);
  }

  private:
  std::istream& stream;
  std::string inputFileName{"test-stream"};
  mcool::Context context{};
};
} // namespace mcool::tests::parser

namespace mcool::tests {
class AttrExtractor {
  public:
  AttrExtractor(std::istream& stream, bool expectedAstStatus = true) {
    driver = std::make_unique<mcool::tests::parser::TestDriver>(stream);
    auto [status, ast] = driver->run();

    assert(status == true);
    assert(ast.isAstOk() == expectedAstStatus);

    classes = ast.get();
    assert(classes != nullptr);
  }

  template <typename T>
  void getAttr(T*& outputAttr, size_t classId, size_t attrId) {
    constexpr auto requestedMethod = std::is_same_v<mcool::ast::SingleMethod, T>;
    constexpr auto requestedMember = std::is_same_v<mcool::ast::SingleMember, T>;
    static_assert(requestedMethod || requestedMember,
                  "requested not method nor member of a class ");

    auto& coolClasses = classes->getData();
    ASSERT_TRUE(classId < coolClasses.size());

    auto classIt = coolClasses.begin();
    std::advance(classIt, classId);

    auto& attrs = (*classIt)->getAttributes()->getData();
    ASSERT_TRUE(attrId < attrs.size());

    auto attrIt = attrs.begin();
    std::advance(attrIt, attrId);
    auto* attrPtr = (*attrIt);

    outputAttr = dynamic_cast<T*>(attrPtr);
    ASSERT_TRUE(outputAttr != nullptr);
  }

  private:
  mcool::ast::CoolClassList* classes{nullptr};
  std::unique_ptr<mcool::tests::parser::TestDriver> driver{nullptr};
};
} // namespace mcool::tests