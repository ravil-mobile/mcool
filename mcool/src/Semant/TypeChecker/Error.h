#pragma once

#include <string>
#include <list>

namespace mcool::semant {
class Error {
  public:
  bool hasErrors() { return !errList.empty(); }

  auto begin() { return errList.begin(); }
  auto end() { return errList.end(); }

  std::list<std::string>& getErrors() { return errList; }

  void retrieveErrors(Error* other) {
    std::copy(other->begin(), other->end(), std::back_inserter(errList));
  }

  protected:
  std::list<std::string> errList{};
};
} // namespace mcool::semant