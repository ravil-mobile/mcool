#include "auxiliary.h"
#include "Semant/TypeChecker/InheritanceGraphBuilder.h"

TEST(Inheritance, NoParentClass) {
  std::stringstream stream;
  stream << "class ClassA inherits ClassC { \n"
         << "  main() : Object {            \n"
         << "    let x: Int in x * 1        \n"
         << "  };                           \n"
         << "};                             \n";

  mcool::tests::semant::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::semant::InheritanceGraphBuilder graphBuilder;
  graphBuilder.build(ast.get());

  ASSERT_TRUE(graphBuilder.hasErrors());
  for (auto& errorMsg: graphBuilder.getErrors()) {
    std::cout << errorMsg << std::endl;
  }
}

TEST(Inheritance, InheritanceWithCycles) {
  std::stringstream stream;
  stream << "class ClassA inherits ClassC { \n"
         << "  main() : Object {            \n"
         << "    let x: Int in x * 1        \n"
         << "  };                           \n"
         << "};                             \n"
         << "class ClassB inherits ClassA { \n"
         << "  foo() : Object {             \n"
         << "    let x: Int in x * 1        \n"
         << "  };                           \n"
         << "};                             \n"
         << "class ClassC inherits ClassB { \n"
         << "  foo() : Object {             \n"
         << "    let x: Int in x * 1        \n"
         << "  };                           \n"
         << "};                             \n";

  mcool::tests::semant::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::semant::InheritanceGraphBuilder graphBuilder;
  graphBuilder.build(ast.get());

  ASSERT_TRUE(graphBuilder.hasErrors());
  for (auto& errorMsg: graphBuilder.getErrors()) {
    std::cout << errorMsg << std::endl;
  }
}

TEST(Inheritance, InheritanceWithoutCycles) {
  std::stringstream stream;
  stream << "class ClassA {                 \n"
         << "  main() : Object {            \n"
         << "    let x: Int in x * 1        \n"
         << "  };                           \n"
         << "};                             \n"
         << "class ClassB inherits ClassA { \n"
         << "  foo() : Object {             \n"
         << "    let x: Int in x * 1        \n"
         << "  };                           \n"
         << "};                             \n"
         << "class ClassC {                 \n"
         << "  foo() : Object {             \n"
         << "    let x: Int in x * 1        \n"
         << "  };                           \n"
         << "};                             \n"
         << "class ClassD inherits ClassA { \n"
         << "  foo() : Object {             \n"
         << "    let x: Int in x * 1        \n"
         << "  };                           \n"
         << "};\n";

  mcool::tests::semant::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::semant::InheritanceGraphBuilder graphBuilder;
  graphBuilder.build(ast.get());

  ASSERT_FALSE(graphBuilder.hasErrors());
  for (auto& errorMsg: graphBuilder.getErrors()) {
    std::cout << errorMsg << std::endl;
  }
}

TEST(Inheritance, childParent) {
  std::stringstream stream;
  stream << "class ClassA {                 \n"
         << "  main() : Object {            \n"
         << "    let x: Int in x * 1        \n"
         << "  };                           \n"
         << "};                             \n"
         << "class ClassB inherits ClassA { \n"
         << "  foo() : Object {             \n"
         << "    let x: Int in x * 1        \n"
         << "  };                           \n"
         << "};                             \n"
         << "class ClassC {                 \n"
         << "  foo() : Object {             \n"
         << "    let x: Int in x * 1        \n"
         << "  };                           \n"
         << "};                             \n"
         << "class ClassD inherits ClassC { \n"
         << "  foo() : Object {             \n"
         << "    let x: Int in x * 1        \n"
         << "  };                           \n"
         << "};\n";

  mcool::tests::semant::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::semant::InheritanceGraphBuilder graphBuilder;
  auto graph = graphBuilder.build(ast.get());

  ASSERT_FALSE(graphBuilder.hasErrors());

  auto& nodes = graph->getNodes();
  const auto& classA = nodes.find("ClassA")->second;
  const auto& classB = nodes.find("ClassB")->second;
  const auto& classD = nodes.find("ClassD")->second;
  const auto& classC = nodes.find("ClassC")->second;

  ASSERT_TRUE(classB.isChildOf(&classA));
  ASSERT_FALSE(classA.isChildOf(&classB));

  ASSERT_TRUE(classD.isChildOf(&classC));
  ASSERT_FALSE(classC.isChildOf(&classD));

  ASSERT_FALSE(classD.isChildOf(&classB));
  ASSERT_FALSE(classB.isChildOf(&classD));

  const auto& object = nodes.find("Object")->second;

  ASSERT_TRUE(classB.isChildOf(&object));
  ASSERT_TRUE(classD.isChildOf(&object));

  ASSERT_FALSE(object.isChildOf(&classA));
  ASSERT_FALSE(object.isChildOf(&classC));

  const auto& integer = nodes.find("Int")->second;
  const auto& boolean = nodes.find("Bool")->second;

  ASSERT_TRUE(integer.isChildOf(&object));
  ASSERT_FALSE(object.isChildOf(&integer));
  ASSERT_FALSE(integer.isChildOf(&boolean));
}