#include "llvm/Support/CommandLine.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/TableGen/Error.h"
#include "llvm/TableGen/Main.h"
#include "llvm/TableGen/Record.h"
#include "llvm/ADT/SmallVector.h"


enum ActionType {
  PrintRecords,
  GenCustomRecord
};

namespace {
  llvm::cl::opt<ActionType> Action(
      llvm::cl::desc("Action to perform:"),
      llvm::cl::values(
          clEnumValN(PrintRecords, "print-records", "Print all records to stdout (default)"),
          clEnumValN(GenCustomRecord, "gen-custom-records", "Generate custom records")
      )
  );
}


void GenCustomDefinitions(llvm::raw_ostream &OS,
                          llvm::RecordKeeper &Records) {
  const auto &Defs = Records.getDefs();
  if (!Defs.empty()) {
    for (auto &Def: Defs) {
      auto &D = Def.second;
      llvm::outs() << "class " << D->getName() << " {\n";
      for (auto& Val: D->getValues()) {
        llvm::outs() << "  " << Val.getType()->getAsString() << " " << Val.getName() << ";\n";
      }
      llvm::outs() << "}; \n";
    }
  }
}

bool CustomTableGenMain(llvm::raw_ostream &OS,
                        llvm::RecordKeeper &Records) {

  switch (Action) {
    case PrintRecords: {
      llvm::outs() << "printing records...\n";
      llvm::outs() << Records;
      break;
    }
    case GenCustomRecord: {
      GenCustomDefinitions(OS, Records);
      break;
    }
    default: {
      llvm::outs() << "no active has been selected\n";
    }
  }

  return true;
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