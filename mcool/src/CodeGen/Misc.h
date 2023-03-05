#pragma once

#include <string>
#include <cassert>

namespace mcool::codegen {
inline std::string getProtoName(const std::string& className) { return "Proto" + className; }

inline std::string getConstructorName(const std::string& className) { return className + "_Init"; }

inline std::string getMethodName(const std::string& className, const std::string& methodName) {
  return className + "_" + methodName;
}

inline std::string getDispatchTableName(const std::string& className) {
  return "DispTable_" + className;
}

inline std::string getDispatchTableTypeName(const std::string& className) {
  auto dispTableName = getDispatchTableName(className);
  return dispTableName + "_type";
}

inline constexpr auto getClassNameTableName() { return "ClassNameTable"; }

inline constexpr auto getClassNameTableTypeName() { return "ClassNameTable_type"; }

} // namespace mcool::codegen