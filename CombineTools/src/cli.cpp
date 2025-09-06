#include "CombineHarvester/CombineTools/interface/cli.hpp"

namespace ch {

boost::program_options::variables_map
ParseChronoSpectraOptions(int argc, char** argv,
                          ChronoSpectraOptions &opts,
                          boost::program_options::options_description &config) {
  config.add_options()
    ("help,h", boost::program_options::bool_switch(&opts.help),
     "Display help information (implicit: true; default: false). No input required for `true`.")
    ("workspace", boost::program_options::value<std::string>(&opts.workspace)->required(),
     "Input ROOT workspace file (REQUIRED).")
    ("datacard", boost::program_options::value<std::string>(&opts.datacard)->required(),
     "Input datacard file for rebinning (REQUIRED).")
    ("output", boost::program_options::value<std::string>(&opts.output)->required(),
     "Output ROOT file for storing results (REQUIRED).")
    ("dataset", boost::program_options::value<std::string>(&opts.dataset)->default_value("data_obs"),
     "Dataset name in the workspace (default: `data_obs`).")
    ("fitresult", boost::program_options::value<std::string>(&opts.fitresult)->default_value(""),
     "Path to RooFitResult file (default: none). Format: `filename:fit_name`.")
    ("postfit", boost::program_options::value<bool>(&opts.postfit)->default_value(false)->implicit_value(true),
     "Enable generation of post-fit histograms (implicit: true; default: false). No input required for `true`. Requires a fit result file.")
    ("skipprefit", boost::program_options::value<bool>(&opts.skipprefit)->default_value(false)->implicit_value(true),
     "Skip generation of pre-fit histograms (implicit: true; default: false). No input required for `true`. At least one of `--postfit` or `!skipprefit` must be enabled.")
    ("samples", boost::program_options::value<unsigned>(&opts.samples)->default_value(2000),
     "Number of samples for uncertainty estimation (default: 2000).")
    ("freeze", boost::program_options::value<std::string>(&opts.freeze_arg)->default_value(""),
     "Freeze parameters during the fit (default: none). Example format: `PARAM1,PARAM2=X`.")
    ("groupBins", boost::program_options::value<std::string>(&opts.groupBinsArg)->default_value(""),
     "Group bins under named groups (default: none). Format: `group1:bin1,bin2;group2:bin3`.")
    ("groupProcs", boost::program_options::value<std::string>(&opts.groupProcsArg)->default_value(""),
     "Group processes under named groups (default: none). Format: `group1:proc1,proc2;group2:proc3`.")
    ("skipObs", boost::program_options::value<bool>(&opts.skipObs)->default_value(false)->implicit_value(true),
     "Do not generate data (observed) histograms (implicit: true; default: false). No input required for `true`.")
    ("getRateCorr", boost::program_options::value<bool>(&opts.getRateCorr)->default_value(true)->implicit_value(true),
     "Compute rate correlation matrices for all grouped and ungrouped bins (implicit: true; default: true). No input required for `true`.")
    ("getHistBinCorr", boost::program_options::value<bool>(&opts.getHistBinCorr)->default_value(true)->implicit_value(true),
     "Compute histogram bin correlation matrices for all grouped and ungrouped processes and bins (implicit: true; default: true). No input required for `true`.")
    ("sepProcHists", boost::program_options::value<bool>(&opts.sepProcHists)->default_value(false)->implicit_value(true),
     "Generate separate histograms for processes within process groups (skipped if false) (implicit: true; default: false). No input required for `true`.")
    ("sepBinHists", boost::program_options::value<bool>(&opts.sepBinHists)->default_value(false)->implicit_value(true),
     "Generate separate histograms for bins within bin groups (skipped if false) (implicit: true; default: false). No input required for `true`.")
    ("sepProcHistBinCorr", boost::program_options::value<bool>(&opts.sepProcHistBinCorr)->default_value(false)->implicit_value(true),
     "Compute separate histogram bin correlations for processes within process groups (skipped if false) (implicit: true; default: false). No input required for `true`.")
    ("sepBinHistBinCorr", boost::program_options::value<bool>(&opts.sepBinHistBinCorr)->default_value(false)->implicit_value(true),
     "Compute separate histogram bin correlations for bins within bin groups (skipped if false) (implicit: true; default: false). No input required for `true`.")
    ("sepBinRateCorr", boost::program_options::value<bool>(&opts.sepBinRateCorr)->default_value(false)->implicit_value(true),
     "Compute separate rate correlations for bins within bin groups (skipped if false) (implicit: true; default: false). No input required for `true`.")
    ("plotShapeSyst", boost::program_options::value<bool>(&opts.plotShapeSyst)->default_value(false)->implicit_value(true),
     "Plot up/dn shape variations for each parameter and save to directory specified by systSaveDir. Skipped for grouped bins or grouped processes. (skipped if false) (implicit: true; default: false). No input required for `true`.")
    ("systSaveDir", boost::program_options::value<std::string>(&opts.systSaveDir)->default_value("shapeSystPlots"),
     "Directory for saving pdf and png plots of systematic shape variations for each parameter.")
    ("logy", boost::program_options::value<bool>(&opts.logy)->default_value(false)->implicit_value(true),
     "Set y-axis to log scale in systematic plots.");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, config), vm);
  if (!opts.help) boost::program_options::notify(vm);
  return vm;
}

} // namespace ch
