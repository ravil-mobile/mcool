#pragma once

#include "Misc.h"
#include "AstTree.h"

namespace mcool {
class ParserDriver{
public:
  bool parse(misc::Config& config);
  mcool::AstTree& getAstTree() { return astTree; }
  void beginScan();
  void endScan();
private:
  mcool::AstTree astTree;
};
}