#include "CombineHarvester/CombineTools/interface/PathTools.h"
#include <cstdlib>
#include <filesystem>
#include <string>

namespace ch {
namespace paths {
namespace fs = std::filesystem;

// Helper function to search for the repository root starting from a path.
static fs::path find_base(fs::path start) {
  while (!start.empty()) {
    if (fs::exists(start / "CombineTools") && fs::exists(start / "CombinePdfs")) {
      return start;
    }
    auto parent = start.parent_path();
    if (parent == start) break;
    start = parent;
  }
  return {};
}

std::string base() {
  static std::string cache = []() {
    if (const char* env = std::getenv("CH_BASE")) {
      return std::string(env);
    }
    fs::path exe;
    try {
      exe = fs::canonical("/proc/self/exe").parent_path();
    } catch (fs::filesystem_error const&) {
      exe = fs::current_path();
    }
    fs::path b = find_base(exe);
    if (!b.empty()) return b.string();
    b = find_base(fs::current_path());
    if (!b.empty()) return b.string();
    return std::string();
  }();
  return cache;
}

std::string auxiliaries() {
  if (const char* env = std::getenv("CH_AUXILIARIES")) {
    return std::string(env);
  }
  return (fs::path(base()) / "auxiliaries").string();
}

std::string input() {
  return (fs::path(base()) / "CombineTools" / "input").string();
}

}  // namespace paths
}  // namespace ch
