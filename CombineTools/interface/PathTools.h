#ifndef CombineTools_PathTools_h
#define CombineTools_PathTools_h
#include <string>

namespace ch {
namespace paths {
// Return the base directory of the CombineHarvester repository.
// Uses the CH_BASE environment variable if set, otherwise attempts to
// discover the location by searching upwards from the current directory.
std::string base();

// Path to the external auxiliaries directory.
std::string auxiliaries();

// Path to the CombineTools input directory.
std::string input();
}
}

#endif
