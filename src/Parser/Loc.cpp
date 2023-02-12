#include "Parser/Loc.h"
#include "ast.h"

namespace mcool {
Loc::Loc(const Loc& first, const Loc& second) {
  assert(first.filename->get() == second.filename->get());

  begin = first.begin < second.begin ? first.begin : second.begin;
  end = first.end > second.end ? first.end : second.end;
  filename = first.filename;
}
} // namespace mcool