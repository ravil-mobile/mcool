#include "Misc.h"
#include "InheritanceBuilder.h"
#include "PrettyTreePrinter.h"
#include "AstCodeEmitter.h"
#include "AttributeBuilder.h"
#include "VisitorCodeEmitter.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/TableGen/Error.h"
#include "llvm/TableGen/Main.h"
#include "llvm/ADT/SmallVector.h"
#include <iostream>

enum ActionType {
  PrintRecords,
  PrintInheritanceTree,
  GenAstHeader,
  GenVisitorHeader
};

namespace {
  llvm::cl::opt<ActionType> Action(
    llvm::cl::desc("Action to perform:"),
    llvm::cl::values(
      clEnumValN(PrintRecords, "print-records", "Print all records to stdout (default)"),
      clEnumValN(PrintInheritanceTree, "print-inheritance", "Print inheritance tree"),
      clEnumValN(GenAstHeader, "gen-ast-header", "Generate Ast hpp code"),
      clEnumValN(GenVisitorHeader, "gen-visitor-header", "Generate Visitor header")
    )
  );
}

void printInheritanceTree(llvm::raw_ostream &OS,
                          std::unique_ptr<ast::inheritance::tree::Node>& tree) {

  ast::PrettyPrinter printer(std::cout, '.');
  tree->accept(&printer);
}

bool generateVisitor(llvm::raw_ostream &OS,
                     std::unique_ptr<ast::inheritance::tree::Node>& tree) {
  OS << "#pragma once\n\n";
  OS << "namespace mcool::ast {\n";

  ast::VisitorCodeEmitter emitter(OS);
  tree->accept(&emitter);

  OS << "} // namespace mcool::ast\n";

  return false;
}

bool generateAstHeader(llvm::raw_ostream &OS,
                       std::unique_ptr<ast::inheritance::tree::Node>& tree) {

  ast::AttributeBuilder builder;
  tree->accept(&builder);

  OS << "#pragma once\n";
  OS << "#include \"visitor.h\"\n";
  OS << "#include \"Parser/Loc.h\"\n";
  OS << "#include \"Types/Types.h\"\n";
  OS << "#include <string>\n";
  OS << "#include <list>\n";
  OS << "\n\n";

  OS << "namespace mcool::ast {\n";

  ast::AstCodeEmitter astEmitter(OS);
  tree->accept(&astEmitter);

  OS << "}\n\n";
  return false;
}


bool customTableGenMain(llvm::raw_ostream &OS,
                        llvm::RecordKeeper &Records) {

  auto records = ast::misc::collectDefsAndClasses(Records);

  ast::inheritance::InheritanceBuilder builder{};
  auto tree = builder.build(records);

  switch (Action) {
    case PrintRecords: {
      llvm::outs() << "printing records...\n";
      llvm::outs() << Records;
      break;
    }
    case PrintInheritanceTree : {
      printInheritanceTree(OS, tree);
      break;
    }
    case GenAstHeader : {
      return generateAstHeader(OS, tree);
    }
    case GenVisitorHeader : {
      return generateVisitor(OS, tree);
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

  return llvm::TableGenMain(argv[0], &customTableGenMain);
}