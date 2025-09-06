#ifndef CombineTools_cli_hpp
#define CombineTools_cli_hpp
#include <string>
#include <boost/program_options.hpp>

namespace ch {

struct ChronoSpectraOptions {
  std::string datacard;
  std::string workspace;
  std::string fitresult;
  std::string output;
  std::string groupBinsArg;
  std::string groupProcsArg;
  std::string freeze_arg;
  std::string dataset;
  std::string systSaveDir;
  unsigned samples;
  bool postfit;
  bool skipprefit;
  bool skipObs;
  bool getRateCorr;
  bool getHistBinCorr;
  bool sepProcHists;
  bool sepBinHists;
  bool sepProcHistBinCorr;
  bool sepBinHistBinCorr;
  bool sepBinRateCorr;
  bool plotShapeSyst;
  bool logy;
  bool help;
};

boost::program_options::variables_map
ParseChronoSpectraOptions(int argc, char** argv,
                          ChronoSpectraOptions &opts,
                          boost::program_options::options_description &config);

} // namespace ch

#endif
