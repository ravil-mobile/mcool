#include "auxiliary.h"
#include "Semant/TypeChecker/EnvironmentsBuilder.h"

TEST(TypeChecking, CheckSimpleTypes) {
  std::stringstream stream;
  stream << "class Test {                                  \n"
         << "  c: Int <- 2;                                \n"
         << "  s: String <- \"hello\";                     \n"
         << "  t0: Test <- new SELF_TYPE;                  \n"
         << "  t1: SELF_TYPE <- self.copy();               \n"
         << "  test1(a: Int, b: Int): Int {                \n"
         << "      a + c * b                               \n"
         << "  };                                          \n"
         << "  test2(a: Int, b: Int): Int {                \n"
         << "    {                                         \n"
         << "      a + c;                                  \n"
         << "      c / 2;                                  \n"
         << "      a * b;                                  \n"
         << "    }                                         \n"
         << "  };                                          \n"
         << "  test3(b: Int): Int {                        \n"
         << "    {                                         \n"
         << "      let t2 : SELF_TYPE in {                 \n"
         << "        t2.copy();                            \n"
         << "      };                                      \n"
         << "      let t3 : SELF_TYPE <- t0.copy() in {    \n"
         << "        b + 4;                                \n"
         << "      };                                      \n"
         << "    }                                         \n"
         << "  };                                          \n"
         << "};                                            \n";

  mcool::tests::semant::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::semant::applyTypeChecking(ast, driver.getContext());
}

TEST(TypeChecking, NewExpr) {
  std::stringstream stream;
  stream << "class Test {                     \n"
         << "  test1(): Int {                 \n"
         << "    new Int                      \n"
         << "  };                             \n"
         << "  test2(): Test {                \n"
         << "    new SELF_TYPE                \n"
         << "  };                             \n"
         << "};                               \n";

  mcool::tests::semant::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::semant::applyTypeChecking(ast, driver.getContext());
}

TEST(TypeChecking, LoopExpr) {
  std::stringstream stream;
  stream << "class Test {                         \n"
         << "  test1(c: Int, a: Int): Object {    \n"
         << "    while c < 4 loop                 \n"
         << "      {                              \n"
         << "        a + 1;                       \n"
         << "      }                              \n"
         << "    pool                             \n"
         << "  };                                 \n"
         << "};                                   \n";

  mcool::tests::semant::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::semant::applyTypeChecking(ast, driver.getContext());
}

TEST(TypeChecking, IfExpr) {
  std::stringstream stream;
  stream << "class A {};                          \n"
         << "class B inherits A {};               \n"
         << "class C inherits A {};               \n"
         << "class D inherits C {                 \n"
         << "  test1(c: C): A {                   \n"
         << "    if not Isvoid c then             \n"
         << "       new D                         \n"
         << "    else                             \n"
         << "       new B                         \n"
         << "    fi                               \n"
         << "  };                                 \n"
         << "};                                   \n";

  mcool::tests::semant::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::semant::applyTypeChecking(ast, driver.getContext());
}

TEST(TypeChecking, AssignExpr) {
  std::stringstream stream;
  stream << "class Test {                         \n"
         << "  a: Int;                            \n"
         << "  test1(): Int {                     \n"
         << "    a <- a + 1                       \n"
         << "  };                                 \n"
         << "};                                   \n";

  mcool::tests::semant::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::semant::applyTypeChecking(ast, driver.getContext());
}

TEST(TypeChecking, LetExpr) {
  std::stringstream stream;
  stream << "class Test {                            \n"
         << "  a: Int;                               \n"
         << "  i: String;                            \n"
         << "  test1(): Int {                        \n"
         << "    let i : Int <- 2, j : Int <- 3 in { \n"
         << "      a <- i * j;                       \n"
         << "    }                                   \n"
         << "  };                                    \n"
         << "};                                      \n";

  mcool::tests::semant::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::semant::applyTypeChecking(ast, driver.getContext());
}

TEST(TypeChecking, MemberDefinitionInParentClass) {
  std::stringstream stream;
  stream << "class Base {                            \n"
         << "  k: Int <- 3;                          \n"
         << "};                                      \n"
         << "class Test inherits Base {              \n"
         << "  a: Int;                               \n"
         << "  test1(): Int {                        \n"
         << "    let i : Int <- 2, j : Int <- 3 in { \n"
         << "      a <- i * j / k;                   \n"
         << "    }                                   \n"
         << "  };                                    \n"
         << "};                                      \n";

  mcool::tests::semant::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::semant::applyTypeChecking(ast, driver.getContext());
}

TEST(TypeChecking, DynamicDispatchTest) {
  std::stringstream stream;
  stream << "class Test {                            \n"
         << "  b: Int <- 7;                          \n"
         << "  test1(a: Int): Int {                  \n"
         << "    let i : Int <- 2, j : Int <- 3 in { \n"
         << "      i + j / a;                        \n"
         << "    }                                   \n"
         << "  };                                    \n"
         << "  test2(): Object {                     \n"
         << "    {                                   \n"
         << "      self.test1(b + 3) + 2;            \n"
         << "      self.copy().abort();              \n"
         << "    }                                   \n"
         << "  };                                    \n"
         << "};                                      \n";

  mcool::tests::semant::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::semant::applyTypeChecking(ast, driver.getContext());
}

TEST(TypeChecking, DynamicDispatchOverloadingTest) {
  std::stringstream stream;
  stream << "\n"
         << "class Base {                            \n"
         << "  number: Int <- 4;                     \n"
         << "  string: String;                       \n"
         << "  test1(b: Int) : String {              \n"
         << "    \"hello\"                           \n"
         << "  };                                    \n"
         << "};                                      \n"
         << "class Test inherits Base {              \n"
         << "  test1(a: Int): Int {                  \n"
         << "    let i : Int <- 2, j : Int <- 3 in { \n"
         << "      i + j / a;                        \n"
         << "    }                                   \n"
         << "  };                                    \n"
         << "  test2(): Object {                     \n"
         << "    {                                   \n"
         << "      number <- test1(4);               \n"
         << "      string <- self@Base.test1(3);     \n"
         << "      abort();                          \n"
         << "    }                                   \n"
         << "  };                                    \n"
         << "};                                      \n";

  mcool::tests::semant::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::semant::applyTypeChecking(ast, driver.getContext());
}

TEST(TypeChecking, CaseTest) {
  std::stringstream stream;
  stream << "                                        \n"
         << "class Test  {                           \n"
         << "  test1(a: Int): Int {                  \n"
         << "    let i : Int <- 2, j : Int <- 3 in { \n"
         << "      i + j / a;                        \n"
         << "    }                                   \n"
         << "  };                                    \n"
         << "  test2(): Int {                        \n"
         << "    case test1(10) of                   \n"
         << "      x: Int => x + 3;                  \n"
         << "      y: String => y.length();          \n"
         << "      z: Test => z.test1(3);            \n"
         << "      k: IO => k.in_int();              \n"
         << "     esac                               \n"
         << "  };                                    \n"
         << "};                                      \n";

  mcool::tests::semant::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::semant::applyTypeChecking(ast, driver.getContext());
}

TEST(TypeChecking, ReturnSelfTest) {
  std::stringstream stream;
  stream << "class C {                        \n"
         << "  a : Int;                       \n"
         << "  b : Bool;                      \n"
         << "  init(x : Int, y : Bool) : C {  \n"
         << "    {                            \n"
         << "      a <- x;                    \n"
         << "      b <- y;                    \n"
         << "      self;                      \n"
         << "    }                            \n"
         << "  };                             \n"
         << "};                               \n";

  mcool::tests::semant::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::semant::applyTypeChecking(ast, driver.getContext());
}
