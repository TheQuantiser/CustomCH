#include <map>
#include "boost/program_options.hpp"
#include "boost/format.hpp"
#include "TSystem.h"
#include "TH2F.h"
#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/ParseCombineWorkspace.h"
#include "CombineHarvester/CombineTools/interface/TFileIO.h"
#include "CombineHarvester/CombineTools/interface/Logging.h"

namespace po = boost::program_options;
using namespace std;
void runSpecificCMB(ch::CombineHarvester & sCMB, ch::CombineHarvester & sCMB_card, map<string, TH1F> & shapeMap, unsigned nsamples=0, RooFitResult* sRes = nullptr, TH2F * cov = nullptr, TH2F * corr = nullptr);
void runAll(ch::CombineHarvester & aCMB, ch::CombineHarvester & aCMB_card, map<string, map<string, TH1F>> & shapeMap, bool FullDataset=1, bool SkipCR=0, unsigned nsamples=0, RooFitResult* res = nullptr, map<string, TH2F> * covMap = nullptr, map<string, TH2F> * corrMap = nullptr);


int main(int argc, char* argv[]) {
  gSystem->Load("libHiggsAnalysisCombinedLimit");
  string datacard   = "";
  string workspace  = "";
  string fitresult  = "";
  string mass       = "125";
  bool postfit      = false;
  bool fullDataset  = false;
  bool skipCR     = false;
  string output     = "";
  unsigned samples  = 500;
  std::string freeze_arg = "";
  string data       = "data_obs";
  po::options_description config("Configuration");
  config.add_options()
  ("workspace,w",
    po::value<string>(&workspace)->required(),
    "The input workspace-containing file [REQUIRED]")
  ("dataset",
    po::value<string>(&data)->default_value(data),
    "The input dataset name")
  ("datacard,d",
    po::value<string>(&datacard)->required(),
    "The input datacard, only used for rebinning")
  ("output,o ",
    po::value<string>(&output)->required(),
    "Name of the output root file to create [REQUIRED]")
  ("fitresult,f",
    po::value<string>(&fitresult)->default_value(fitresult),
    "Path to a RooFitResult, only needed for postfit")
  ("postfit",
    po::value<bool>(&postfit)
    ->default_value(postfit)->implicit_value(true),
    "Create post-fit histograms in addition to pre-fit")
  ("samples",
    po::value<unsigned>(&samples)->default_value(samples),
    "Number of samples to make in each evaluate call")
  ("freeze",
    po::value<string>(&freeze_arg)->default_value(freeze_arg),
    "Format PARAM1,PARAM2=X,PARAM3=Y where the values X and Y are optional")
  ("fullDataset",
    po::value<bool>(&fullDataset)
    ->default_value(fullDataset)->implicit_value(false),
    "Run over full dataset only")
  ("skipCR",
    po::value<bool>(&skipCR)
    ->default_value(skipCR)->implicit_value(false),
    "Skip control regions");

  po::variables_map vm;

  // Parse the main config options
  po::store(po::command_line_parser(argc, argv).options(config).run(), vm);
  po::notify(vm);

  if(samples==0) cout<<">> Will not use sampling for uncertainties and covariance\n";
  
  // Load workspace
  TFile infile(workspace.c_str());
  RooWorkspace *ws = dynamic_cast<RooWorkspace*>(gDirectory->Get("w"));

  if (!ws) throw std::runtime_error(  FNERROR("Could not locate workspace in input file"));

  if(fullDataset){
    std::cout<<"Will make plots for the combined dataset only!"<<std::endl;
  }

  if(skipCR){
    std::cout<<"Skipping control regions"<<std::endl;
  }

  // Create CH instance from workspace
  ch::CombineHarvester cmb;
  cmb.SetFlag("workspaces-use-clone", true);
  ch::ParseCombineWorkspace(cmb, *ws, "ModelConfig", data, false);

  // Only evaluate in case parameters to freeze are provided
  if(! freeze_arg.empty()) {
    vector<string> freeze_vec;
    boost::split(freeze_vec, freeze_arg, boost::is_any_of(","));
    for (auto const& item : freeze_vec) {
      vector<string> parts;
      boost::split(parts, item, boost::is_any_of("="));
      if (parts.size() == 1) {
        ch::Parameter *par = cmb.GetParameter(parts[0]);
        if (par) par->set_frozen(true);
        else throw std::runtime_error(FNERROR("Requested variable to freeze does not exist in workspace"));
      } else {
        if (parts.size() == 2) {
          ch::Parameter *par = cmb.GetParameter(parts[0]);
          if (par) {
            par->set_val(boost::lexical_cast<double>(parts[1]));
            par->set_frozen(true);
            std::cout<<"Freezing parameter  "<<parts[0]<<" to "<<parts[1]<<std::endl;
          }
          else throw std::runtime_error(FNERROR("Requested variable to freeze does not exist in workspace"));
        }
      }
    }
  }


  // Create CH instance from datacard (for rebinning)
  ch::CombineHarvester cmb_card;
  cmb_card.SetFlag("workspaces-use-clone",true);
  cmb_card.ParseDatacard(datacard, "", "", "", 0, mass);

  // Drop any process that has no hist/data/pdf
  cmb.FilterProcs([&](ch::Process * proc) {
    bool no_shape = !proc->shape() && !proc->data() && !proc->pdf();
    if (no_shape) {
      cout << "Filtering process with no shape:\n";
      cout << ch::Process::PrintHeader << *proc << "\n";
    }
    return no_shape;
  });


  TFile outfile(output.c_str(), "RECREATE");
  TH1::AddDirectory(false);

  map<string, map<string, TH1F>>          prefitShapes;
  std::cout << "\n\n>> Doing prefit..."<<std::endl;
  runAll(cmb, cmb_card, prefitShapes, fullDataset, skipCR);

  map<string, map<string, TH1F>>          postfitShapes;
  map<string, TH2F>               postfitCov;
  map<string, TH2F>               postfitCorr;

  if (postfit) {

    std::cout <<"\n\n>> Doing postfit..."<<std::endl;
      // Get the fit result and update the parameters to the post-fit model
    RooFitResult res = ch::OpenFromTFile<RooFitResult>(fitresult);
    cmb.UpdateParameters(res);

    // Only evaluate in case parameters to freeze are provided
    if(! freeze_arg.empty()) {
      vector<string> freeze_vec;
      boost::split(freeze_vec, freeze_arg, boost::is_any_of(","));
      for (auto const& item : freeze_vec) {
        vector<string> parts;
        boost::split(parts, item, boost::is_any_of("="));
        if (parts.size() == 1) {
          ch::Parameter *par = cmb.GetParameter(parts[0]);
          if (par) par->set_frozen(true);
          else throw std::runtime_error(FNERROR("Requested variable to freeze does not exist in workspace"));
        } else {
          if (parts.size() == 2) {
            ch::Parameter *par = cmb.GetParameter(parts[0]);
            if (par) {
              par->set_val(boost::lexical_cast<double>(parts[1]));
              par->set_frozen(true);
              std::cout<<"Freezing parameter  "<<parts[0]<<" to "<<parts[1]<<std::endl;
            }
            else throw std::runtime_error(FNERROR("Requested variable to freeze does not exist in workspace"));
          }
        }
      }
    }

    runAll(cmb, cmb_card, postfitShapes, fullDataset, skipCR, samples, &res, &postfitCov, &postfitCorr);
  }

  for (auto& iter1 : prefitShapes) {
    for (auto& iter2 : iter1.second) {
      ch::WriteToTFile(&(iter2.second), &outfile, std::string("prefit/") + iter1.first + "/" + iter2.first);
      if (postfit) ch::WriteToTFile(&(postfitShapes[iter1.first][iter2.first]), &outfile, std::string("postfit/") + iter1.first + "/" + iter2.first); 
    }
  }

  if (postfit) {
    for (auto& it : postfitCov) {
      ch::WriteToTFile(&(it.second), &outfile, std::string("postfit/") + it.first + "/" + it.first+"_cov"); 
      ch::WriteToTFile(&(postfitCorr[it.first]), &outfile, std::string("postfit/") + it.first + "/" + it.first+"_corr");  
    }
  }

  outfile.Close();

  cout<<"ZNuNuGPrePostFit task complete!"<<endl;
  return 0;
};

void runAll(ch::CombineHarvester & aCMB, ch::CombineHarvester & aCMB_card, map<string, map<string, TH1F>> & shapeMap, bool FullDataset, bool SkipCR, unsigned nsamples, RooFitResult* res, map<string, TH2F> * covMap, map<string, TH2F> * corrMap) {
  //// split regions and combine years
  for(auto iPhaseSpace : vector<string>{"EB", "EE"}){
    ch::CombineHarvester phaseSpace_aCMB = aCMB.cp().bin_rgx({std::string(".*") + iPhaseSpace + ".*"});
    if(phaseSpace_aCMB.cp().bin_set().size() == 0) continue;
    for(auto iRegion : vector<string>{"CRe", "CRmu", "SR"}){
      if(SkipCR && ((iRegion.find("CRe") != std::string::npos) || (iRegion.find("CRmu") != std::string::npos))){
        continue;
      }
      std::cout << "\n>> Doing "<<iPhaseSpace<<" "<< iRegion<<"..."<<std::endl;
      ch::CombineHarvester region_aCMB = phaseSpace_aCMB.cp().bin_rgx({std::string(".*") + iRegion + ".*"});
      string tmpName= iPhaseSpace + "_" + iRegion;
      if(res && (nsamples >0) && covMap && corrMap) runSpecificCMB(region_aCMB, aCMB_card, shapeMap[tmpName], nsamples, res, &((*covMap)[tmpName]), &((*corrMap)[tmpName]));
      else runSpecificCMB(region_aCMB, aCMB_card, shapeMap[tmpName]);
    }
  }
  if(FullDataset) return;
  for (auto bin : aCMB.cp().bin_set()) {
    if(SkipCR && ((bin.find("CRe") != std::string::npos) || (bin.find("CRmu") != std::string::npos))){
      continue;
    }
    std::cout << "\n>> Doing "<<bin<<"..."<<std::endl;
    ch::CombineHarvester aCMB_bin = aCMB.cp().bin({bin});
    if(res && (nsamples >0) && covMap && corrMap) runSpecificCMB(aCMB_bin, aCMB_card, shapeMap[bin], nsamples, res, &((*covMap)[bin]), &((*corrMap)[bin]));
    else runSpecificCMB(aCMB_bin, aCMB_card, shapeMap[bin]);
  }
};

void runSpecificCMB(ch::CombineHarvester & sCMB, ch::CombineHarvester & sCMB_card, map<string, TH1F> & shapeMap, unsigned nsamples, RooFitResult* sRes, TH2F * cov, TH2F * corr) {
  shapeMap["data_obs"] = sCMB.GetObservedShape();
  // std::cout << ">>>> Bkg rate = " << sCMB.cp().backgrounds().GetRate() << " +- "<< sCMB.cp().backgrounds().GetUncertainty() <<std::endl;
  std::cout << ">>>> data_obs rate = " << shapeMap["data_obs"].Integral()<<std::endl;
  std::cout << ">>>> Bkg rate = " << sCMB.cp().backgrounds().GetRate()<<std::endl;
  shapeMap["TotalBkg"] = (nsamples>0) ? sCMB.cp().backgrounds().GetShapeWithUncertainty(*sRes, nsamples) : sCMB.cp().backgrounds().GetShapeWithUncertainty();
  // std::cout << ">>>> Sig rate = " << sCMB.cp().signals().GetRate() << " +- "<< sCMB.cp().signals().GetUncertainty() <<std::endl;
  std::cout << ">>>> Sig rate = " << sCMB.cp().signals().GetRate() <<std::endl;
  shapeMap["TotalSig"] = (nsamples>0) ? sCMB.cp().signals().GetShapeWithUncertainty(*sRes, nsamples) : sCMB.cp().signals().GetShapeWithUncertainty();
  // std::cout << ">>>> All processeses rate = " <<sCMB.cp().GetRate() << " +- "<< sCMB.cp().GetUncertainty() <<std::endl;
  std::cout << ">>>> All processeses rate = " <<sCMB.cp().GetRate() <<std::endl;
  shapeMap["TotalProcs"] = (nsamples>0) ? sCMB.GetShapeWithUncertainty(*sRes, nsamples) : sCMB.GetShapeWithUncertainty();
  for (auto proc : sCMB.process_set()) {
    ch::CombineHarvester procCMB = sCMB.cp().process({proc});
    // std::cout << ">>>> " << proc << " rate = "<<procCMB.GetRate() << " +- "<< procCMB.GetUncertainty() <<std::endl;
    std::cout << ">>>> " << proc << " rate = "<<procCMB.GetRate() <<std::endl;
    shapeMap[proc] = (nsamples>0) ? procCMB.GetShapeWithUncertainty(*sRes, nsamples) : procCMB.GetShapeWithUncertainty();
  }
    // Summ W+G and get full shape
  ch::CombineHarvester sCMB_tot_WG = sCMB.cp().process_rgx({".*WLNuG.*",".*WG.*"});
  // std::cout << ">>>> Merged WLNuG rate "<< sCMB_tot_WG.GetRate() << " +- "<< sCMB_tot_WG.GetUncertainty() <<std::endl;
  std::cout << ">>>> Merged WLNuG rate "<< sCMB_tot_WG.GetRate() <<std::endl;
  shapeMap["mergedWLNuG"] = (nsamples>0) ? sCMB_tot_WG.GetShapeWithUncertainty(*sRes, nsamples) : sCMB_tot_WG.GetShapeWithUncertainty();


  // Summ fiducial and get full shape
  ch::CombineHarvester sCMB_tot_fiducial = sCMB.cp().process_rgx({".*fiducial_.*"});
  // std::cout << ">>>> Merged WLNuG rate "<< sCMB_tot_WG.GetRate() << " +- "<< sCMB_tot_WG.GetUncertainty() <<std::endl;
  std::cout << ">>>> Merged fiducial rate "<< sCMB_tot_fiducial.GetRate() <<std::endl;
  shapeMap["mergedFiducialZNuNuG"] = (nsamples>0) ? sCMB_tot_fiducial.GetShapeWithUncertainty(*sRes, nsamples) : sCMB_tot_fiducial.GetShapeWithUncertainty();

  // Sum aNTGC, aNTGC+SM
  ///quad_cG   quad_cGtil sm        sm_lin_quad_cG sm_lin_quad_cGtil sm_lin_quad_mixed_cG_cGtil minor_bkg beamHalo  eleFakes  jetFakes  WLNuG_Bin1 WLNuG_Bin2 WLNuG_Bin3 WLNuG_Bin4 WLNuG_Bin5 WLNuG_Bin6 WLNuG_Bin7
  ch::CombineHarvester sm_plus_antgc = sCMB.cp().process_rgx({"quad_cG","quad_cGtil","sm*","sm_lin_quad_cG","sm_lin_quad_cGtil","sm_lin_quad_mixed_cG_cGtil"});
  std::cout << ">>>> sm_plus_antgc rate "<< sm_plus_antgc.GetRate() <<std::endl;
  shapeMap["sm_plus_antgc"] = (nsamples>0) ? sm_plus_antgc.GetShapeWithUncertainty(*sRes, nsamples) : sm_plus_antgc.GetShapeWithUncertainty();

  ch::CombineHarvester antgc_only = sCMB.cp().process_rgx({"quad_cG","quad_cGtil","sm_lin_quad_cG","sm_lin_quad_cGtil","sm_lin_quad_mixed_cG_cGtil"});
  std::cout << ">>>> antgc_only rate "<< antgc_only.GetRate() <<std::endl;
  shapeMap["antgc_only"] = (nsamples>0) ? antgc_only.GetShapeWithUncertainty(*sRes, nsamples) : antgc_only.GetShapeWithUncertainty();

  // ch::CombineHarvester all_bkg = sCMB.cp().process_rgx({"minor_bkg", "beamHalo",  "eleFakes",  "jetFakes",  ".*WLNuG.*",".*WG.*", "spikes"});
  // std::cout << ">>>> all_bkg rate "<< all_bkg.GetRate() <<std::endl;
  // shapeMap["all_bkg"] = (nsamples>0) ? all_bkg.GetShapeWithUncertainty(*sRes, nsamples) : all_bkg.GetShapeWithUncertainty(); 


  TH1F ref = sCMB_card.cp().bin({*(sCMB.bin_set().begin())}).GetObservedShape();
  for (auto & it : shapeMap) {
    it.second = ch::RestoreBinning(it.second, ref);
  }
  if(nsamples>0 && sRes && cov && corr){
    *cov = sCMB.GetRateCovariance(*sRes, nsamples);
    *corr = sCMB.GetRateCorrelation(*sRes, nsamples);
  }
};