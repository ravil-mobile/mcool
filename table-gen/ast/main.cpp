#include "Misc.h"
#include "Generate.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/TableGen/Error.h"
#include "llvm/TableGen/Main.h"
#include "llvm/ADT/SmallVector.h"


enum ActionType {
  PrintRecords,
  GenAstCpp,
  GenAstHeader,
  GenVisitorHeader
};

namespace {
  llvm::cl::opt<ActionType> Action(
      llvm::cl::desc("Action to perform:"),
      llvm::cl::values(
          clEnumValN(PrintRecords, "print-records", "Print all records to stdout (default)"),
          clEnumValN(GenAstCpp, "gen-ast-cpp", "Generate Ast cpp code"),
          clEnumValN(GenAstHeader, "gen-ast-header", "Generate Ast hpp code"),
          clEnumValN(GenVisitorHeader, "gen-visitor-header", "Generate Visitor header")
    )
  );
}

void generateAstCpp(llvm::raw_ostream &OS,
                    llvm::RecordKeeper &Records) {
  const auto &Defs = Records.getDefs();
  OS << "#pragma once\n";
  OS << "using namespace std;\n";

  OS << "namespace mcool::ast {\n";
  if (!Defs.empty()) {
    for (auto &Def: Defs) {
      auto &D = Def.second;
      OS << "class " << D->getName() << " {\n";
      OS << "}; \n";
    }
  }
  OS << "} // namespace mcool::ast\n";
}

bool generateVisitor(llvm::raw_ostream &OS, llvm::RecordKeeper &Records) {
  OS << "#pragma once\n\n";
  OS << "namespace mcool::ast {\n";

  const auto &Definitions = Records.getDefs();
  if (!Definitions.empty()) {
    for (auto &Defenition: Definitions) {
      OS << "class " << Defenition.second->getName() << ";\n";
    }
  }

  OS << '\n' << "class Visitor {\n";
  OS << "public:\n";
  OS << "  virtual ~Visitor() = default;\n";
  if (!Definitions.empty()) {
    for (auto &Defenition: Definitions) {
      auto name = Defenition.second->getName();
      OS << "  virtual void visit"
         << ast::misc::capitalize(name.str())
         << "(" << name << "*) {};\n";
    }
  }

  OS << "};\n";
  OS << "} // namespace mcool::ast\n";
  return false;
}

bool generateAstHeader(llvm::raw_ostream &OS,
                       llvm::RecordKeeper &Records) {
  OS << "#pragma once\n";
  OS << "#include \"visitor.h\"\n";
  OS << "#include <string>\n";
  OS << "#include <list>\n";
  OS << "\n\n";

  OS << "namespace mcool::ast {\n";

  OS << "class Node {\n";
  OS << "  public:\n";
  OS << "  virtual ~Node() = default;\n";
  OS << "  virtual void accept(Visitor*) = 0;\n";
  OS << "  virtual bool isTerminal() = 0;\n";
  OS << "  virtual std::string getName() = 0;\n";
  OS << "};\n";

  OS << "class Terminal : public Node {};\n";
  OS << "class NonTerminal : public Node {};\n\n";

  const auto &Definitions = Records.getDefs();

  if (!Definitions.empty()) {
    for (auto &Defenition: Definitions) {
      auto &D = Defenition.second;
      auto* recordType = D->getType();

      auto astKind = ast::misc::getKind(recordType);
      if (not astKind.has_value()) {
        llvm::errs() << "an ast node should be derived either from Terminal or NonTerminal";
        return true;
      }

      /*
      auto parentName = ast::misc::getParentName(recordType);
      if (not parentName.has_value()) {
        llvm::errs() << "cannot find the parent class";
        return true;
      }
      */

      auto definitionName = D->getName();
      std::string parentName = (*astKind) == ast::NodeKind::Terminal ? "Terminal" : "NonTerminal";
      OS << "class " << definitionName << " : public " << parentName << " {\n";
      OS << "public: \n";

      auto attributes = ast::misc::getAttributes(D->getValues());
      ast::genConstructor(OS, definitionName.str(), attributes);

      OS << "  void accept(Visitor* visitor) override { visitor->visit" << definitionName << "(this); }\n";
      if ((*astKind) == ast::NodeKind::Terminal) {
        OS << "  bool isTerminal() override { return true; }\n";
      }
      else {
        OS << "  bool isTerminal() override { return false; }\n";
      }

      OS << "  std::string getName() override { return std::string{\"" << definitionName <<  "\"}; }\n";

      ast::genGetters(OS, attributes);
      ast::genSetters(OS, attributes);

      OS << "\n";
      for (auto& attr : attributes) {
        OS << "  " << attr.type->getName() << " " << attr.name;
        if (attr.initValue) {
          OS << "{" << *(attr.initValue) << "}";
        }
        OS << ";\n";
      }

      OS << "};\n\n";
    }
  }
  OS << "} // namespace mcool::ast\n";
  return false;
}

bool CustomTableGenMain(llvm::raw_ostream &OS,
                        llvm::RecordKeeper &Records) {

  switch (Action) {
    case PrintRecords: {
      llvm::outs() << "printing records...\n";
      llvm::outs() << Records;
      break;
    }
    case GenAstCpp: {
      generateAstCpp(OS, Records);
      break;
    }
    case GenAstHeader: {
      return generateAstHeader(OS, Records);
    }
    case GenVisitorHeader: {
      return generateVisitor(OS, Records);
    }
    default: {
      llvm::outs() << "no active has been selected\n";
    }
  }

  return false;
}


// https://llvm.org/docs/TableGen/ProgRef.html
// https://llvm.org/docs/TableGen/BackGuide.html
int main(int argc, char **argv) {
  llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);
  llvm::PrettyStackTraceProgram X(argc, argv);
  llvm::cl::ParseCommandLineOptions(argc, argv);

  llvm::llvm_shutdown_obj Y;

  return llvm::TableGenMain(argv[0], &CustomTableGenMain);
}