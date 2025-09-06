#include "CombineHarvester/CombineTools/interface/cli.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/any.hpp>
#include <iostream>

ChronoSpectraConfig parseCommandLine(int argc, char *argv[]) {
  ChronoSpectraConfig cfg;
  bool show_help = false;
  namespace po = boost::program_options;
  po::options_description config("Allowed Options");
  config.add_options()
      ("help,h", po::bool_switch(&show_help),
       "Display help information (implicit: true; default: false). No input required for `true`.")
      ("workspace", po::value<std::string>(&cfg.workspace)->required(),
       "Input ROOT workspace file (REQUIRED).")
      ("datacard", po::value<std::string>(&cfg.datacard)->required(),
       "Input datacard file for rebinning (REQUIRED).")
      ("output", po::value<std::string>(&cfg.output)->required(),
       "Output ROOT file for storing results (REQUIRED).")
      ("dataset", po::value<std::string>(&cfg.dataset)->default_value("data_obs"),
       "Dataset name in the workspace (default: `data_obs`).")
      ("fitresult", po::value<std::string>(&cfg.fitresult)->default_value(""),
       "Path to RooFitResult file (default: none). Format: `filename:fit_name`.")
      ("postfit", po::value<bool>(&cfg.postfit)->default_value(false)->implicit_value(true),
       "Enable generation of post-fit histograms (implicit: true; default: false). No input required for `true`. Requires a fit result file.")
      ("skipprefit", po::value<bool>(&cfg.skipprefit)->default_value(false)->implicit_value(true),
       "Skip generation of pre-fit histograms (implicit: true; default: false). No input required for `true`. At least one of `--postfit` or `!skipprefit` must be enabled.")
      ("samples", po::value<unsigned>(&cfg.samples)->default_value(2000),
       "Number of samples for uncertainty estimation (default: 2000).")
      ("freeze", po::value<std::string>(&cfg.freeze_arg)->default_value(""),
       "Freeze parameters during the fit (default: none). Example format: `PARAM1,PARAM2=X`.")
      ("groupBins", po::value<std::string>(&cfg.groupBinsArg)->default_value(""),
       "Group bins under named groups (default: none). Format: `group1:bin1,bin2;group2:bin3`.")
      ("groupProcs", po::value<std::string>(&cfg.groupProcsArg)->default_value(""),
       "Group processes under named groups (default: none). Format: `group1:proc1,proc2;group2:proc3`.")
      ("skipObs", po::value<bool>(&cfg.skipObs)->default_value(false)->implicit_value(true),
       "Do not generate data (observed) histograms (implicit: true; default: false). No input required for `true`.")
      ("getRateCorr", po::value<bool>(&cfg.getRateCorr)->default_value(true)->implicit_value(true),
       "Compute rate correlation matrices for all grouped and ungrouped bins (implicit: true; default: true). No input required for `true`.")
      ("getHistBinCorr", po::value<bool>(&cfg.getHistBinCorr)->default_value(true)->implicit_value(true),
       "Compute histogram bin correlation matrices for all grouped and ungrouped processes and bins (implicit: true; default: true). No input required for `true`.")
      ("sepProcHists", po::value<bool>(&cfg.sepProcHists)->default_value(false)->implicit_value(true),
       "Generate separate histograms for processes within process groups (skipped if false) (implicit: true; default: false). No input required for `true`.")
      ("sepBinHists", po::value<bool>(&cfg.sepBinHists)->default_value(false)->implicit_value(true),
       "Generate separate histograms for bins within bin groups (skipped if false) (implicit: true; default: false). No input required for `true`.")
      ("sepProcHistBinCorr", po::value<bool>(&cfg.sepProcHistBinCorr)->default_value(false)->implicit_value(true),
       "Compute separate histogram bin correlations for processes within process groups (skipped if false) (implicit: true; default: false). No input required for `true`.")
      ("sepBinHistBinCorr", po::value<bool>(&cfg.sepBinHistBinCorr)->default_value(false)->implicit_value(true),
       "Compute separate histogram bin correlations for bins within bin groups (skipped if false) (implicit: true; default: false). No input required for `true`.")
      ("sepBinRateCorr", po::value<bool>(&cfg.sepBinRateCorr)->default_value(false)->implicit_value(true),
       "Compute separate rate correlations for bins within bin groups (skipped if false) (implicit: true; default: false). No input required for `true`.")
      ("storeSyst", po::value<bool>(&cfg.storeSyst)->default_value(false)->implicit_value(true),
       "Store up/dn shape variations for each parameter in the output ROOT file. Skipped for grouped bins or grouped processes. (implicit: true; default: false). No input required for `true`.")
      ("plotSyst", po::value<std::string>(&cfg.plotSystArg)->default_value(""),
       "Plot stored shape variations. Accepts 'all' or a comma-separated list of 'bin/process/systematic' patterns with '*' wildcards (regex-style '.\*' also supported). Missing combinations are skipped silently.")
      ("systSaveDir", po::value<std::string>(&cfg.systSaveDir)->default_value(cfg.systSaveDir),
       "Directory for saving pdf and png plots of systematic shape variations for each parameter.")
      ("logy", po::value<bool>(&cfg.logy)->default_value(false)->implicit_value(true),
       "Set y-axis to log scale in systematic plots.");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, config), vm);

  if (vm.count("help") && vm["help"].as<bool>()) {
    std::cout << "\n" << config << std::endl
              << "\nExample Usage:\n"
              << "ChronoSpectra --workspace workspace.root --datacard datacard.txt "
                 "--output output.root --dataset data_obs --postfit "
              << "--fitResult=fit.root:fit_mdf --samples 2000 --freeze Wrate=1.5,pdf "
              << "--groupBins 'region1: bin1, bin2; region2: bin3, bin4' "
              << "-groupProcs 'type1:procA,procB;type2:procC,procD' "
              << "--skipObs --getRateCorr=false --getHistBinCorr --skipprefit "
              << "--sepProcHists --sepBinHists --sepProcHistBinCorr "
                 "--sepBinHistBinCorr --sepBinRateCorr --plotSyst=binA/proc*/syst1\n";
    std::exit(0);
  }

  po::notify(vm);

  if (!cfg.plotSystArg.empty()) {
    if (cfg.plotSystArg == "all" || cfg.plotSystArg == "*/*/*") {
      cfg.plotSystAll = true;
    } else {
      std::vector<std::string> pats;
      boost::split(pats, cfg.plotSystArg, boost::is_any_of(","));
      for (auto &p : pats) {
        boost::trim(p);
        if (p.empty())
          continue;
        if (p == "*/*/*") {
          cfg.plotSystAll = true;
          continue;
        }
        std::vector<std::string> parts;
        boost::split(parts, p, boost::is_any_of("/"));
        if (parts.size() != 3)
          continue;
        for (auto &part : parts) {
          boost::trim(part);
          boost::replace_all(part, ".*", "*");
        }
        cfg.plotSystPatterns.insert(parts[0] + "/" + parts[1] + "/" + parts[2]);
      }
    }
  }

  std::cout << "\n\nUsing option values:" << std::endl;
  for (const auto &option : config.options()) {
    const std::string &name = option->long_name();
    std::cout << "--" << name << ": ";
    if (vm.count(name)) {
      try {
        auto value = vm[name].value();
        if (value.type() == typeid(std::string)) {
          std::cout << boost::any_cast<std::string>(value);
        } else if (value.type() == typeid(bool)) {
          std::cout << (boost::any_cast<bool>(value) ? "true" : "false");
        } else if (value.type() == typeid(unsigned)) {
          std::cout << boost::any_cast<unsigned>(value);
        } else {
          std::cout << "unknown value type";
        }
      } catch (const boost::bad_any_cast &) {
        std::cout << "Error casting value";
      }
    } else {
      std::cout << "not set";
    }
    std::cout << std::endl;
  }
  std::cout << "\n\n";

  return cfg;
}

