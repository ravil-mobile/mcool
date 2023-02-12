#include "CodeGen/CodeGenDriver.h"
#include <fstream>

namespace mcool::codegen {
bool CodeGenDriver::run(mcool::AstTree& classes) {
  std::fstream os(config.outputFile + ".o", std::ios::out);
  os << "generating source code..." << std::endl;

  os.close();
  return true;
}
} // namespace mcool::codegen