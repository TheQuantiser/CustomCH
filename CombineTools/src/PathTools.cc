#include "CombineHarvester/CombineTools/interface/PathTools.h"
#include <cstdlib>
#include <string>
#include <boost/filesystem.hpp>

namespace ch {
namespace paths {

std::string base() {
  const char* env = std::getenv("CH_BASE");
  if (env) return std::string(env);
  boost::filesystem::path p = boost::filesystem::current_path();
  while (true) {
    if (boost::filesystem::exists(p / "CombineTools") &&
        boost::filesystem::exists(p / "CombinePdfs")) {
      return p.string();
    }
    if (p.has_parent_path()) {
      p = p.parent_path();
    } else {
      break;
    }
  }
  return "";
}

std::string auxiliaries() {
  return base() + "/../auxiliaries/";
}

std::string input() {
  return base() + "/CombineTools/input";
}

}  // namespace paths
}  // namespace ch
