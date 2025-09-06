#ifndef CHRONOSPECTRA_CLI_HPP
#define CHRONOSPECTRA_CLI_HPP

#include <set>
#include <string>

struct ChronoSpectraConfig {
  std::string datacard;
  std::string workspace;
  std::string output;
  std::string dataset = "data_obs";
  std::string fitresult;
  unsigned samples = 2000;
  bool postfit = false;
  bool skipprefit = false;
  std::string freeze_arg;
  std::string groupBinsArg;
  std::string groupProcsArg;
  bool skipObs = false;
  bool getRateCorr = true;
  bool getHistBinCorr = true;
  bool sepProcHists = false;
  bool sepBinHists = false;
  bool sepProcHistBinCorr = false;
  bool sepBinHistBinCorr = false;
  bool sepBinRateCorr = false;
  bool storeSyst = false;
  std::string plotSystArg;
  std::set<std::string> plotSystPatterns;
  bool plotSystAll = false;
  std::string systSaveDir = "shapeSystPlots";
  bool logy = false;
};

ChronoSpectraConfig parseCommandLine(int argc, char *argv[]);

#endif  // CHRONOSPECTRA_CLI_HPP
