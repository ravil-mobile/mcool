#pragma once

#include <string>
#include <sstream>

namespace mcool {
inline void resetStream(std::stringstream& stream, std::string&& str) {
  stream.str(str);
  stream.seekp(0, std::ios_base::end);
}

inline void removeFromBack(std::stringstream& stream, size_t numChar) {
  if (numChar == 0) return;

  auto str = stream.str();
  if (numChar < str.size()) {
    str.erase(str.size() - numChar, numChar);
    mcool::resetStream(stream, std::move(str));
  }
}
} // namespace mcool