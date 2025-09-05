/* ============================================================================
 ChronoSpectra.cpp
 Author: Mohammad Abrar Wadud, 2024
 ============================================================================
 Efficient Pre-fit & Post-fit Histogram Extraction for CMS Physics Analyses
 ============================================================================

 ChronoSpectra License (Creative Commons Attribution 4.0 International - CC
BY 4.0)

 Copyright (c) 2024 Mohammad Abrar Wadud

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software (the "Software"), to use, copy, modify, merge, publish,
 and distribute the Software for personal, academic, research, or commercial
 purposes, subject to the following conditions:

 1. Attribution:
    - Proper credit must be given to the original author, including citing this
      Software in any publications, presentations, or projects that directly
      or indirectly use the Software.
    - Include a clear reference to the Software's official repository (if
applicable).

 2. Use for Derivative Works:
    - If this Software is modified, extended, or used as inspiration for other
      works, acknowledgment must be given to the original author.
    - Any derivative work must include a notice stating that the original
Software has been modified, along with a description of the modifications.

 3. Commercial Use:
    - This Software may be used in commercial products or services, provided
that appropriate credit is given in any associated documentation or promotional
      materials.

 4. Disclaimer of Warranty:
    - This Software is provided "as is," without warranty of any kind, express
      or implied, including but not limited to warranties of merchantability,
      fitness for a particular purpose, and non-infringement.

 5. Limitation of Liability:
    - In no event shall the author or copyright holder be liable for any claim,
      damages, or other liability, whether in an action of contract, tort, or
      otherwise, arising from, out of, or in connection with the Software or
      the use or other dealings in the Software.

 6. License Reference:
    - This license is governed by the Creative Commons Attribution 4.0
      International License (CC BY 4.0). For more details, visit:
      https://creativecommons.org/licenses/by/4.0/

 ============================================================================

 Purpose:
 --------
 ChronoSpectra simplifies the extraction and analysis of pre-fit and post-fit
 histograms from Combine-generated ROOT workspaces. It enhances traditional
 approaches with flexibles features such as flexible grouping of bins and
 processes, and detailed correlation matrix computation. Inspired by the widely
 used "PostFitShapesFromWorkspace.cpp" in CombineHarvester,
 ChronoSpectra builds on its foundation to offer more organized outputs,
 structured analysis options, and improved usability.

 ==========================
 Installation instructions:
 ==========================
    git clone https://github.com/TheQuantiser/CustomCH.git
    cd CustomCH
    git submodule update --init
    cmake -S . -B build
    cmake --build build --target install
    export CH_BASE=$(pwd)
    source build/setup.sh

 ---------
 Features:
 ---------
 1. Pre-fit & Post-fit Histograms
    - Pre-fit: Reflects the model before parameter optimization.
    - Post-fit: Incorporates adjustments from a provided RooFitResult.

 2. Histogram Grouping
    - Group bins/processes with user-defined names for structured output.
    - Supports ungrouped individual bin/process handling.

 3. Uncertainty Estimation
    - Enables random sampling to quantify uncertainties.

 4. Correlation Matrices
    - Computes correlation matrices for histogram bin-to-bin and
process-to-process rates.

 5. Parameter Freezing
    - Allows fixing parameters with optional values for custom fits.

 6. Organized Output
    - Saves histograms and matrices in structured directories: `prefit/`,
`postfit/`.
    - Provides flexibility for grouped or individual histogram handling.

 ---------------------
 Command-Line Options:
 ---------------------
 --help / -h              : Display help information (implicit: true; default:
false). No input required for `true`.
 --workspace (REQUIRED)   : Input ROOT workspace file.
 --datacard (REQUIRED)    : Input datacard file for rebinning.
 --output (REQUIRED)      : Output ROOT file for storing results.
 --dataset                : Dataset name in the workspace (default: `data_obs`).
 --fitresult              : Path to RooFitResult file (default: none).
                             Format: `filename:fit_name`.
 --postfit                : Enable generation of post-fit histograms
                             (implicit: true; default: false).
                             No input required for `true`. Requires a fit result
file.
 --skipprefit             : Skip generation of pre-fit histograms
                             (implicit: true; default: false).
                             No input required for `true`. At least one of
                             `--postfit` or `!skipprefit` must be enabled.
 --samples                : Number of samples for uncertainty estimation
(default: 0).
 --freeze                 : Freeze parameters during the fit (default: none).
                             Example format: `PARAM1,PARAM2=X`.
 --groupBins              : Group bins under named groups (default: none).
                             Format: `group1:bin1,bin2;group2:bin3`.
 --groupProcs             : Group processes under named groups (default: none).
                             Format: `group1:proc1,proc2;group2:proc3`.
 --skipObs                : Do not generate data (observed) histograms
                             (implicit: true; default: false).
                             No input required for `true`.
 --getRateCorr            : Compute rate correlation matrices for all grouped
and ungrouped bins (implicit: true; default: true). No input required for
`true`.
 --getHistBinCorr         : Compute histogram bin correlation matrices for all
grouped and ungrouped processes and bins (implicit: true; default: true). No
input required for `true`.
 --sepProcHists           : Generate separate histograms for processes within
process groups (skipped if false) (implicit: true; default: false). No input
required for `true`.
 --sepBinHists            : Generate separate histograms for bins within bin
groups (skipped if false) (implicit: true; default: false). No input required
for `true`.
 --sepProcHistBinCorr     : Compute separate histogram bin correlations for
processess within process groups (skipped if false) (implicit: true; default:
false). No input required for `true`.
 --sepBinHistBinCorr      : Compute separate histogram bin correlations for bins
within bin groups (skipped if false) (implicit: true; default: false). No input
required for `true`.
 --sepBinRateCorr         : Compute separate rate correlations for bins within
bin groups (skipped if false) (implicit: true; default: false). No input
required for `true`.
  --storeSyst              : Store up/dn shape variations for each parameter in
the output ROOT file (skipped if false) (implicit: true; default: false). No
input required for `true`.
  --plotSyst               : Plot stored shape variations. Accepts `all` or a
comma-separated list of `bin/process/systematic` patterns with `*` wildcards;
missing combinations are skipped silently.
  --systSaveDir             : Directory for saving pdf and png plots of
systematic shape variations for each parameter.

 --------------
 Example Usage:
 --------------
ChronoSpectra --help \
--workspace workspace.root --datacard datacard.txt --output output.root \
--dataset data_obs --postfit --fitresult=fit.root:fit_mdf --samples 2000 \
--freeze Wrate=1.5,pdf --groupBins "region1:bin1,bin2;region2:bin3,bin4" \
--groupProcs "signal:procA,procB;background:procC,procD" --skipObs \
--getRateCorr=false --getHistBinCorr --skipprefit \
--sepProcHists --sepBinHists --sepProcHistBinCorr --sepBinHistBinCorr \
--sepBinRateCorr --plotSyst=binA/procX/syst1

 Output Structure:
 -----------------
 A root file specified by `--output` containing:
 1. Histograms: Stored under `prefit/` and `postfit/` directories.
 2. Correlation Matrices: Organized as `<bin>/<process>_HistBinCorr` or
`_RateCorr`.

 Notes:
 ------
 - Ensure workspace and datacard files align to avoid mismatches.
 - Use `--samples` for sampling-based uncertainty estimation.
 - Results are saved in a structured and organized ROOT file.
**/

#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/ParseCombineWorkspace.h"
#include "CombineHarvester/CombineTools/interface/TFileIO.h"
#include "CombineTools/interface/cli.hpp"
#include "TROOT.h"
#include <TCanvas.h>
#include <TGaxis.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TLine.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TSystem.h>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <memory>
#include <map>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <expected>
#include <stdexcept>
#include <RooFitResult.h>

// Commonly used container types
using HistMap = std::map<std::string, std::map<std::string, TH1F>>;
using MatrixMap = std::map<std::string, std::map<std::string, TH2F>>;

// Enumeration to control optional computations for a bin/process pair
enum class ProcTask : uint8_t { Histogram = 1 << 0, RateCorr = 1 << 1, BinCorr = 1 << 2 };
using ProcTaskFlags = uint8_t;

inline bool hasTask(ProcTaskFlags flags, ProcTask task) {
  return (flags & static_cast<ProcTaskFlags>(task)) != 0;
}

inline ProcTaskFlags makeTasks(bool hist, bool rate, bool bin) {
  ProcTaskFlags flags = 0;
  if (hist)
    flags |= static_cast<ProcTaskFlags>(ProcTask::Histogram);
  if (rate)
    flags |= static_cast<ProcTaskFlags>(ProcTask::RateCorr);
  if (bin)
    flags |= static_cast<ProcTaskFlags>(ProcTask::BinCorr);
  return flags;
}

// Context bundle for process computations
struct ProcessContext {
  bool doSamplingUnc;
  unsigned samples;
  RooFitResult *fitRes;
  HistMap &histograms;
  MatrixMap *RateCorrMap;
  MatrixMap *HistBinCorrMap;
};

std::string printTimestamp() {
  auto now = std::chrono::system_clock::now();
  auto time_t_now = std::chrono::system_clock::to_time_t(now);
  std::tm local_tm = *std::localtime(&time_t_now);

  std::ostringstream timestamp;
  timestamp << "[" << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S") << "]";
  return timestamp.str();
}

void displayStartupMessage() {
  std::cout << "\n\n\n\n"
            << printTimestamp() << "\tStarting ChronoSpectra (c) MAW 2024 \n\n";

  // ASCII Art for ChronoSpectra
  std::cout << R"(
           _   _   _   _   _   _      
          / \ / \ / \ / \ / \ / \
         ( C | H | R | O | N | O )    
          \_/ \_/ \_/ \_/ \_/ \_/  _  
          / \ / \ / \ / \ / \ / \ / \
         ( S | P | E | C | T | R | A )
          \_/ \_/ \_/ \_/ \_/ \_/ \_/ 
        )" << "\n";

  // License Information
  std::cout
      << "==============================================================\n";
  std::cout << "      ChronoSpectra (c) 2024 Mohammad Abrar Wadud\n";
  std::cout << "  Efficient Pre-fit & Post-fit Histogram Extraction for CMS\n";
  std::cout
      << "==============================================================\n\n";

  std::cout
      << "  Licensed under Creative Commons Attribution 4.0 (CC BY 4.0).\n";
  std::cout << "  You are free to use, modify, and distribute this software,\n";
  std::cout << "  provided appropriate credit is given.\n\n";

  std::cout
      << "  Full License: https://creativecommons.org/licenses/by/4.0/\n\n";
  std::cout << "  Official repository: "
               "https://github.com/TheQuantiser/CombineHarvester/blob/main/"
               "CombineTools/bin/ChronoSpectra.cpp\n";
  std::cout
      << "==============================================================\n\n";
};

// Function to apply style settings to a TH2F
void ApplyTH2FStyle(TH2F &matrix) {
  matrix.SetOption("colz");
  matrix.SetDrawOption("colz");
  matrix.SetContour(2000);
  matrix.GetXaxis()->LabelsOption("v");
  matrix.GetZaxis()->SetMoreLogLabels();
}

// Function: parseNamedGroups
// --------------------------
// Parses named groups of bins or processes from the command-line argument
// format. Example format: "group1:bin1,bin2;group2:bin3,bin4" Input:
// - groupsArg: A string containing semicolon-separated group definitions with
// names. Output:
// - A map where keys are group names, and values are vectors of items in the
// group.
using NamedGroupResult =
    std::expected<std::map<std::string, std::vector<std::string>>, std::vector<std::string>>;

NamedGroupResult parseNamedGroups(const std::string &groupsArg) {
  std::map<std::string, std::vector<std::string>> namedGroups;
  std::vector<std::string> diagnostics;

  if (groupsArg.empty())
    return namedGroups;

  std::vector<std::string> groupStrings;
  boost::split(groupStrings, groupsArg, boost::is_any_of(";"));

  for (std::string &group : groupStrings) {
    boost::trim(group);

    size_t colonPos = group.find(':');
    if (colonPos == std::string::npos || colonPos == 0 ||
        colonPos == group.size() - 1) {
      diagnostics.push_back("Invalid group format: '" + group +
                             "' (missing or misplaced ':')");
      continue;
    }

    std::string groupName = group.substr(0, colonPos);
    boost::trim(groupName);

    if (groupName.empty() || groupName.find(' ') != std::string::npos) {
      diagnostics.push_back("Invalid group name: '" + groupName + "'");
      continue;
    }

    auto [it, inserted] =
        namedGroups.emplace(groupName, std::vector<std::string>{});
    if (!inserted) {
      diagnostics.push_back("Duplicate group name found: '" + groupName + "'");
      continue;
    }

    std::vector<std::string> items;
    boost::split(items, group.substr(colonPos + 1), boost::is_any_of(","));
    items.erase(std::remove_if(items.begin(), items.end(),
                               [](const std::string &item) {
                                 return item.empty() ||
                                        item.find(' ') != std::string::npos;
                               }),
                items.end());

    if (items.empty()) {
      namedGroups.erase(groupName);
      diagnostics.push_back("Group '" + groupName +
                            "' contains no valid items.");
      continue;
    }

    it->second = std::move(items);
  }

  if (!diagnostics.empty()) {
    return std::unexpected(diagnostics);
  }

  // Print out the groups and their elements
  for (const auto &[groupName, items] : namedGroups) {
    std::cout << "Group: '" << groupName << "' -> [ ";
    for (size_t i = 0; i < items.size(); ++i) {
      std::cout << "'" << items[i] << "'";
      if (i != items.size() - 1)
        std::cout << ", ";
    }
    std::cout << " ]\n";
  }

  return namedGroups;
}

struct SystHists {
  TH1F nominal;
  TH1F up;
  TH1F down;
};

SystHists BuildSystHists(ch::CombineHarvester &cmb, ch::Parameter &param) {
  double original_val = param.val();
  double err_u = param.err_u();
  double err_d = param.err_d();
  auto CreateShape = [&](double value) {
    param.set_val(value);
    return cmb.GetShape();
  };
  TH1F nominal = CreateShape(original_val);
  TH1F up = CreateShape(original_val + err_u);
  TH1F down = CreateShape(original_val + err_d);
  param.set_val(original_val);
  return {nominal, up, down};
}

bool WildcardMatch(const std::string &pattern, const std::string &value) {
  std::string regexPattern = "^";
  for (char c : pattern) {
    if (c == '*')
      regexPattern += ".*";
    else if (std::isalnum(static_cast<unsigned char>(c)) || c == '_')
      regexPattern.push_back(c);
    else {
      regexPattern += '\\';
      regexPattern.push_back(c);
    }
  }
  regexPattern += "$";
  return std::regex_match(value, std::regex(regexPattern));
}

// Check if a given bin/process/systematic combination should be plotted
bool ShouldPlot(const std::string &bin, const std::string &proc,
                const std::string &syst,
                const ChronoSpectraConfig &cfg) {
  if (cfg.plotSystAll)
    return true;
  if (cfg.plotSystPatterns.empty())
    return false;
  for (const auto &pat : cfg.plotSystPatterns) {
    auto first = pat.find('/');
    auto second = pat.find('/', first + 1);
    if (first == std::string::npos || second == std::string::npos)
      continue;
    std::string binPat = pat.substr(0, first);
    std::string procPat = pat.substr(first + 1, second - first - 1);
    std::string systPat = pat.substr(second + 1);
    if (WildcardMatch(binPat, bin) && WildcardMatch(procPat, proc) &&
        WildcardMatch(systPat, syst))
      return true;
  }
  return false;
}

void plotShapeSystVariations(const SystHists &hists,
                             const std::string &paramName,
                             const std::string &saveName,
                             const ChronoSpectraConfig &cfg) {
  TH1F nominal = hists.nominal;
  TH1F up = hists.up;
  TH1F down = hists.down;

  const double nominal_integral = nominal.Integral();
  const double up_integral = up.Integral();
  const double down_integral = down.Integral();

  const double skip_thres = 1E-15 * std::abs(nominal_integral);

  if (!(nominal_integral > 0.) ||
      (std::abs(nominal_integral - up_integral) < skip_thres &&
       std::abs(down_integral - nominal_integral) < skip_thres &&
       std::abs(up_integral - down_integral) < skip_thres))
    return;

  std::string plotName = saveName + "_" + paramName;

  // if (!((plotName.find("jmr") != std::string::npos ||
  //         plotName.find("jms") != std::string::npos ||
  //         plotName.find("mcstat_bin") != std::string::npos) &&
  //         (plotName.find("ZH") != std::string::npos ||
  //          plotName.find("WH") != std::string::npos ||
  //          plotName.find("TTbar") != std::string::npos ||
  //          plotName.find("WJetsLNu") != std::string::npos
  //         )
  //      )) return;

  // Create canvas
  TCanvas canvas("canvas", "canvas", 2800, 2400);

  TGaxis::SetExponentOffset(-0.15, -0.15, "y");

  // Upper pad (log scale)
  TPad pad0("pad0", "", 0., 0.4, 1., 1.);
  pad0.SetLeftMargin(0.25);
  pad0.SetRightMargin(0.05);
  pad0.SetBottomMargin(0.015);
  pad0.SetTopMargin(0.1);
  pad0.SetGrid(1, 1);
  pad0.Draw();
  pad0.cd();

  nominal.SetTitle(plotName.c_str());
  nominal.GetYaxis()->SetMoreLogLabels();
  nominal.GetYaxis()->SetTitle("Events/bin");
  nominal.GetYaxis()->CenterTitle();
  nominal.GetYaxis()->SetTitleOffset(0.96);
  nominal.GetXaxis()->SetLabelSize(0.);
  nominal.GetXaxis()->SetTitleSize(0.);
  nominal.GetYaxis()->SetTitleSize(0.1);
  nominal.GetYaxis()->SetLabelSize(0.085);
  nominal.SetLineColor(kBlack);
  nominal.SetLineWidth(5);
  up.SetLineColor(kRed);
  up.SetLineWidth(5);
  down.SetLineColor(kGreen);
  down.SetLineWidth(5);

  nominal.Draw("hist");
  up.Draw("hist same");
  down.Draw("hist same");

  // Lambda to calculate min/max
  auto GetHistMinMax = [](const std::vector<TH1F *> &hists) {
    double minVal = std::numeric_limits<double>::max();
    double maxVal = -std::numeric_limits<double>::max();
    for (const auto *hist : hists) {
      minVal = std::min(minVal, hist->GetMinimum());
      maxVal = std::max(maxVal, hist->GetMaximum());
    }
    return std::make_pair(minVal, maxVal);
  };

  auto [yMin, yMax] = GetHistMinMax({&nominal, &up, &down});

  if (cfg.logy) {
    pad0.SetLogy();
    yMin = std::max(yMin * 0.8, 0.01); // Apply padding and avoid zero
    yMax *= 1.2;                       // Apply upper padding
  } else {
    double yMinPadding = 0.05;
    double yMaxPadding = 0.05;
    double linUnit = (yMax - yMin) / (1.0 - yMinPadding - yMaxPadding);
    yMin = yMin - yMinPadding * linUnit;
    yMax = yMax + yMaxPadding * linUnit;
  }

  nominal.SetMinimum(yMin);
  nominal.SetMaximum(yMax);

  nominal.Draw("hist");
  up.Draw("hist same");
  down.Draw("hist same");

  pad0.RedrawAxis();
  pad0.Update();
  pad0.Modified();

  TPaveText *hTitle = (TPaveText *)(pad0.GetPrimitive("title"));
  hTitle->SetTextSize(0.06);
  pad0.Modified();

  TLegend legend(0.6, 0.67, 0.95, 0.9);
  legend.SetNColumns(1);
  legend.SetTextSize(0.046);
  legend.SetFillStyle(1000);
  legend.SetFillColor(kGray);

  // Lambda function to determine the decimal position of the smallest
  // significant difference in a list of double values.
  auto GetSigDecPos = [](const std::vector<double> &values) -> int {
    if (values.size() < 2)
      return -1; // Not enough numbers to compare

    double minDiff = std::numeric_limits<double>::max();

    for (size_t i = 0; i < values.size(); ++i) {
      double abs_val_i = std::abs(values[i]);
      for (size_t j = i + 1; j < values.size(); ++j) {
        double abs_val_j = std::abs(values[j]);
        double diff = std::abs(abs_val_i - abs_val_j);
        diff = std::min(std::min(abs_val_i, abs_val_j), diff);
        if (diff > 0.0 && diff < minDiff) {
          minDiff = diff;
        }
      }
    }

    if (minDiff == std::numeric_limits<double>::max())
      return -1;

    int decimalPosition = -static_cast<int>(std::round(std::log10(minDiff)));

    return decimalPosition;
  };

  // Lambda function to format a double value with a specified number of decimal
  // positions This function takes an integer `ndec` and a double `x`, formats
  // `x` to `ndec` decimal places, and returns the formatted number as a string.
  auto FormatDecPos = [](int ndec, double x) -> std::string {
    std::ostringstream stream;

    // Calculate the rounding factor as 10 raised to the power of `ndec`.
    double factor = std::pow(10.0, ndec);

    // Apply rounding logic using the calculated factor.
    x = std::round(x * factor) / factor;

    // Determine precision for positive `ndec`.
    int precision = (ndec > 0) ? ndec : 0;

    // Format the number with fixed-point notation and set precision
    stream << std::fixed << std::setprecision(precision) << x;

    // Return the formatted string
    return stream.str();
  };

  // Add legend entries
  int ndecpos1 =
      GetSigDecPos({nominal_integral, up_integral, down_integral}) + 1;
  ndecpos1 = std::min(std::abs(ndecpos1), 2);
  double up_integral_deviation =
      100. * (up_integral - nominal_integral) / nominal_integral;
  double down_integral_deviation =
      100. * (down_integral - nominal_integral) / nominal_integral;
  int ndecpos2 =
      GetSigDecPos({down_integral_deviation, up_integral_deviation}) + 1;
  ndecpos2 = (ndecpos2 > 0) ? std::min(ndecpos2, 2) : 0;
  legend.AddEntry(
      &nominal,
      ("Nominal (n= " + FormatDecPos(ndecpos1, nominal_integral) + ")").c_str(),
      "l");
  legend.AddEntry(&up,
                  ("Up (n= " + FormatDecPos(ndecpos1, up_integral) + ", " +
                   (up_integral_deviation > 0 ? "+" : "") +
                   FormatDecPos(ndecpos2, up_integral_deviation) + "%)")
                      .c_str(),
                  "l");
  legend.AddEntry(&down,
                  ("Down (n=" + FormatDecPos(ndecpos1, down_integral) + ", " +
                   (down_integral_deviation > 0 ? "+" : "") +
                   FormatDecPos(ndecpos2, down_integral_deviation) + "%)")
                      .c_str(),
                  "l");
  legend.Draw();

  canvas.cd();

  // Lower pad (linear scale)
  TPad pad1("pad1", "", 0., 0., 1., 0.4);
  pad1.SetLeftMargin(0.25);
  pad1.SetRightMargin(0.05);
  pad1.SetBottomMargin(0.38);
  pad1.SetTopMargin(0.0);
  pad1.SetGrid(1, 1);
  pad1.Draw();
  pad1.cd();

  TH1F rel_diff_up = TH1F(up);
  TH1F rel_diff_down = TH1F(down);
  rel_diff_up.SetName("rel_diff_up");
  rel_diff_down.SetName("rel_diff_down");
  rel_diff_up.Reset();
  rel_diff_down.Reset();
  // Calculate relative differences
  for (int bin = 1; bin <= rel_diff_up.GetNbinsX(); ++bin) {
    double nom_val = nominal.GetBinContent(bin);
    if (nom_val > 0) {
      rel_diff_up.SetBinContent(bin, 100. * (up.GetBinContent(bin) - nom_val) /
                                         nom_val);
      rel_diff_down.SetBinContent(
          bin, 100. * (down.GetBinContent(bin) - nom_val) / nom_val);
    } else {
      rel_diff_up.SetBinContent(bin, 0);   // Unfilled behavior
      rel_diff_down.SetBinContent(bin, 0); // Unfilled behavior
    }
  }

  rel_diff_up.SetLineColor(kRed);
  rel_diff_down.SetLineColor(kGreen);
  rel_diff_up.SetLineWidth(5);
  rel_diff_down.SetLineWidth(5);
  rel_diff_up.GetYaxis()->SetTitle("#splitline{Variation}{    (%)}");
  rel_diff_up.GetYaxis()->CenterTitle();
  rel_diff_up.GetXaxis()->CenterTitle();
  rel_diff_up.GetYaxis()->SetTitleOffset(0.67);
  rel_diff_up.GetXaxis()->SetTitleOffset(1.);
  rel_diff_up.GetXaxis()->SetTitleSize(0.145);
  rel_diff_up.GetYaxis()->SetTitleSize(0.145);
  rel_diff_up.GetXaxis()->SetLabelSize(0.13);
  rel_diff_up.GetYaxis()->SetLabelSize(0.13);
  rel_diff_up.GetYaxis()->SetNdivisions(505);
  rel_diff_up.SetTitle("");
  rel_diff_up.GetYaxis()->SetMaxDigits(3);

  rel_diff_up.Draw("hist");

  double xMin = rel_diff_up.GetXaxis()->GetXmin();
  double xMax = rel_diff_up.GetXaxis()->GetXmax();
  TLine line(xMin, 0.0, xMax, 0.0);
  line.SetLineColor(kBlack);
  line.SetLineWidth(6);

  auto [rMin, rMax] = GetHistMinMax({&rel_diff_up, &rel_diff_down});
  double rOffset = std::abs(rMax - rMin) * 0.2;
  rMin = std::min(-rOffset, rMin - rOffset);
  rMin = std::min(rMin, -0.01);
  rMax = std::max(rOffset, rMax + rOffset);
  if (std::abs(rMin - 1.) < 1E-8 && std::abs(rMax - 1.) < 1E-8) {
    rMin = -5.2;
    rMax = 5.2;
  }
  rel_diff_up.SetMinimum(rMin);
  rel_diff_up.SetMaximum(rMax);
  rel_diff_up.SetMinimum(rMin);
  rel_diff_up.SetMaximum(rMax);

  rel_diff_up.Draw("hist");
  line.Draw();
  rel_diff_down.Draw("hist same");
  pad1.RedrawAxis();
  pad1.Update();
  pad1.Modified();

  canvas.SaveAs((cfg.systSaveDir + "/" + plotName + ".png").c_str());
}

void StoreSystematics(ch::CombineHarvester &cmb, const std::string &bin,
                      const std::string &proc, TFile &outfile,
                      std::set<std::string> &writtenNominals,
                      const ChronoSpectraConfig &cfg) {
  std::string key = bin + "/" + proc;
  if (writtenNominals.insert(key).second) {
    TH1F nominal = cmb.GetShape();
    ch::WriteToTFile(&nominal, &outfile, "systematics/" + bin + "/" + proc);
  }
  for (const auto &paramObj : cmb.GetParameters()) {
    ch::Parameter *p = cmb.GetParameter(paramObj.name());
    if (!p)
      continue;
    SystHists hists = BuildSystHists(cmb, *p);
    double nominal_integral = hists.nominal.Integral();
    double up_integral = hists.up.Integral();
    double down_integral = hists.down.Integral();
    double skip_thres = 1E-15 * std::abs(nominal_integral);
    if (!(nominal_integral > 0.) ||
        (std::abs(nominal_integral - up_integral) < skip_thres &&
         std::abs(down_integral - nominal_integral) < skip_thres &&
         std::abs(up_integral - down_integral) < skip_thres))
      continue;
    std::string base = "systematics/" + bin + "/" + proc + "_syst/" + p->name();
    ch::WriteToTFile(&hists.up, &outfile, base + "_Up");
    ch::WriteToTFile(&hists.down, &outfile, base + "_Down");
    // Plot variations only for selected bin/process/systematic combinations
    if (ShouldPlot(bin, proc, p->name(), cfg)) {
      plotShapeSystVariations(hists, p->name(), bin + "_" + proc, cfg);
    }
  }
}

void writeHistogramsToFile(
    std::map<std::string, std::map<std::string, TH1F>> &histograms,
    TFile &outfile, const std::string &prefix) {
  std::cout << printTimestamp()
            << " Writing histograms to file: " << outfile.GetName()
            << std::endl;

  for (auto &[binName, procMap] : histograms) {
    for (auto &[procName, histogram] : procMap) {
      std::string path = prefix + "/" + binName + "/" + procName;
      histogram.SetTitle(procName.c_str());
      std::cout << printTimestamp() << "\t--> " << std::setw(50) << std::left
                << path << " = " << histogram.Integral() << " ± "
                << histogram.GetBinContent(0) << std::endl;
      ch::WriteToTFile(&histogram, &outfile, path);
    }
  }

  // Clear histograms map to ensure memory release
  histograms.clear();

  std::cout << printTimestamp() << " ... done." << std::endl;
}

void writeCorrToFile(
    std::map<std::string, std::map<std::string, TH2F>> &matrixMap,
    TFile &outfile, const std::string &prefix, const std::string &suffix) {
  // Log the start of the process
  std::cout << "\n"
            << printTimestamp()
            << " Writing correlation matrices to file: " << outfile.GetName()
            << std::endl;

  // Iterate through bins and processes
  for (auto &[binName, procMap] : matrixMap) {
    for (auto &[procName, matrix] : procMap) {
      // Construct the path and log it
      std::string path = prefix + "/" + binName + "/" + procName + suffix;
      std::cout << printTimestamp() << "\t--> " << path << std::endl;

      ApplyTH2FStyle(matrix);

      // Write the matrix to the output file
      ch::WriteToTFile(&matrix, &outfile, path);
    }
  }

  // Clear histograms map to ensure memory release
  matrixMap.clear();

  std::cout << printTimestamp() << " ... done." << std::endl;
}

/** Helper to log results for a specific bin/process combination. */
void logResult(const std::string &binName, const std::string &procName,
               const std::string &msg) {
  std::cout << printTimestamp() << std::setw(50) << std::left
            << std::string("\t") + binName + "/" + procName + " " << msg
            << std::endl;
}

/** Create a histogram for a bin/process pair and store it in the context. */
void createHistogram(ch::CombineHarvester &subCmb, const std::string &binName,
                     const std::string &procName, ProcessContext &ctx) {
  ctx.histograms[binName][procName] =
      ctx.doSamplingUnc
          ? subCmb.cp().GetShapeWithUncertainty(*ctx.fitRes, ctx.samples)
          : subCmb.cp().GetShapeWithUncertainty();
  const TH1F &tmp = ctx.histograms[binName][procName];
  std::ostringstream ss;
  ss << "-> " << tmp.Integral() << " ± " << tmp.GetBinContent(0);
  logResult(binName, procName, ss.str());
}

/** Compute rate correlation matrix for a bin/process pair if requested. */
void createRateCorrelation(ch::CombineHarvester &subCmb,
                           const std::string &binName,
                           const std::string &procName, ProcessContext &ctx) {
  if (ctx.RateCorrMap && ctx.doSamplingUnc) {
    (*ctx.RateCorrMap)[binName][procName] =
        subCmb.cp().GetRateCorrelation(*ctx.fitRes, ctx.samples);
    logResult(binName, procName, "rate correlation computed");
  }
}

/** Compute histogram bin correlation matrix for a bin/process pair if requested. */
void createBinCorrelation(ch::CombineHarvester &subCmb,
                          const std::string &binName,
                          const std::string &procName, ProcessContext &ctx) {
  if (ctx.HistBinCorrMap && ctx.doSamplingUnc) {
    (*ctx.HistBinCorrMap)[binName][procName] =
        subCmb.cp().GetHistogramBinCorrelation(*ctx.fitRes, ctx.samples);
    logResult(binName, procName, "histogram bin correlation computed");
  }
}

/** Run selected tasks for a given bin/process combination. */
void computeProcess(ch::CombineHarvester &subCmb, const std::string &binName,
                    const std::string &procName, ProcTaskFlags tasks,
                    ProcessContext &ctx) {
  if (subCmb.process_set().empty())
    return;
  if (hasTask(tasks, ProcTask::Histogram))
    createHistogram(subCmb, binName, procName, ctx);
  if (hasTask(tasks, ProcTask::RateCorr))
    createRateCorrelation(subCmb, binName, procName, ctx);
  if (hasTask(tasks, ProcTask::BinCorr))
    createBinCorrelation(subCmb, binName, procName, ctx);
}

// Function to process all histograms, rate correlations, and bin correlations
// Based on the provided CombineHarvester object and input parameters
void processAll(
    ch::CombineHarvester &cmb,
    std::map<std::string, std::map<std::string, TH1F>> &histograms,
    const std::map<std::string, std::vector<std::string>> &binGroups,
    const std::map<std::string, std::vector<std::string>> &processGroups,
    const ChronoSpectraConfig &cfg,
    unsigned samples = 0, RooFitResult *fitRes = nullptr,
    std::map<std::string, std::map<std::string, TH2F>> *RateCorrMap = nullptr,
    std::map<std::string, std::map<std::string, TH2F>> *HistBinCorrMap =
        nullptr,
    TFile *systFile = nullptr) {

  // Check if post-fit parameters are available
  bool isPostfit = (fitRes != nullptr);

  std::cout << "\n\n"
            << printTimestamp() << "Generating "
            << (isPostfit ? "post-fit" : "pre-fit") << " results..."
            << std::endl;

  // Change the model parameters and uncertainties to the fitted values
  if (isPostfit)
    cmb.UpdateParameters(fitRes);

  // Determine whether to apply sampling uncertainties
  bool doSamplingUnc = isPostfit & (samples > 0);

  std::set<std::string> systNominals;

  ProcessContext ctx{doSamplingUnc, samples, fitRes, histograms, RateCorrMap,
                     HistBinCorrMap};

  // Lambda: Process all computations for a bin or bin group
  auto computeBin = [&](ch::CombineHarvester &binCmb,
                        const std::string &binName, bool doBinHists = true,
                        bool doBinRateCorr = true,
                        bool doBinHistBinCorr = true) -> void {
    if (!doBinHists && !doBinRateCorr && !doBinHistBinCorr)
      return;

    // Log processing start
    std::cout << "\n\n"
              << printTimestamp() << std::setw(50) << std::left
              << " Processing bin/bin group: " << binName << std::endl;

    // Check if the bin contains any processes
    if (binCmb.cp().process_set().empty()) {
      std::cerr << "Warning: Bin/bin group '" << binName
                << "' has no processes." << std::endl;
      return;
    }

    // Process total, signals, and backgrounds
    ProcTaskFlags binTasks = makeTasks(doBinHists, doBinRateCorr, doBinHistBinCorr);
    computeProcess(binCmb.cp().signals(), binName, "signal", binTasks, ctx);
    computeProcess(binCmb.cp().backgrounds(), binName, "background", binTasks, ctx);
    computeProcess(binCmb, binName, "total", binTasks, ctx);

    // Handle observed or pseudo-data
    if (doBinHists) {
      if (cfg.skipObs) {
        histograms[binName][cfg.dataset] = TH1F(histograms[binName]["total"]);
        histograms[binName][cfg.dataset].SetName(cfg.dataset.c_str());
      } else {
        histograms[binName][cfg.dataset] = binCmb.cp().GetObservedShape();
      }

      // Update dataset histogram properties
      auto &obsHist = histograms[binName][cfg.dataset];
      obsHist.SetBinContent(0, std::sqrt(obsHist.Integral()));
      obsHist.SetBinErrorOption(TH1::kPoisson);

      std::ostringstream ss;
      ss << "-> " << obsHist.Integral() << " ± " << obsHist.GetBinContent(0);
      logResult(binName, cfg.dataset + (cfg.skipObs ? " (pseudo-data)" : ""),
                ss.str());
    }

    // Process grouped processes
    std::unordered_set<std::string> processedProcesses;

    for (const auto &[procGroupName, processList] : processGroups) {
      ch::CombineHarvester procGroupCmb = binCmb.cp().process_rgx(processList);

      // Skip empty process groups
      if (procGroupCmb.cp().process_set().empty()) {
        std::cerr << "Warning: Process group '" << procGroupName
                  << "' has no matching processes." << std::endl;
        continue;
      }

      // Compute grouped processes
      computeProcess(procGroupCmb, binName, procGroupName, binTasks, ctx);

      // Log and track processes within the group
      std::cout << printTimestamp() << "\t-- Process group " << procGroupName
                << " contains ";
      for (const auto &proc : procGroupCmb.cp().process_set()) {
        std::cout << proc << ", ";
        processedProcesses.insert(proc);
      }
      std::cout << std::endl;
    }

    // Process ungrouped individual processes
    for (const auto &proc : binCmb.cp().process_set()) {
      bool isProcGrouped = processedProcesses.count(proc) > 0;

      ch::CombineHarvester singleProcCmb = binCmb.cp().process({proc});

      // Plot prefit systematic shape variations for each parameter
      if (cfg.storeSyst && systFile && !isPostfit &&
          (singleProcCmb.bin_set().size() == 1)) {
        StoreSystematics(singleProcCmb, binName, proc, *systFile, systNominals,
                         cfg);
      }

      // Skip grouped processes unless explicitly required
      if (isProcGrouped && !cfg.sepProcHists && !cfg.sepProcHistBinCorr)
        continue;

      if (singleProcCmb.cp().process_set().empty()) {
        std::cerr << "Warning: Process '" << proc << "' not found."
                  << std::endl;
        continue;
      }

      // Compute ungrouped processes
      ProcTaskFlags procTasks = makeTasks(
          doBinHists && (!isProcGrouped || cfg.sepProcHists), false,
          doBinHistBinCorr && (!isProcGrouped || cfg.sepProcHistBinCorr));
      computeProcess(singleProcCmb, binName, proc, procTasks, ctx);
    }
  };

  // Track processed bins
  std::unordered_set<std::string> processedBins;

  // Process bin groups
  for (const auto &[binGroupName, binGroupList] : binGroups) {

    // Create a CombineHarvester for the bin group
    ch::CombineHarvester binCmb = cmb.cp().bin_rgx(binGroupList);

    // Skip if no matching bins
    if (binCmb.cp().bin_set().empty()) {
      std::cerr << "Warning: Bin group '" << binGroupName
                << "' has no matching bins!" << std::endl;
      continue;
    }

    // Compute bin statistics
    computeBin(binCmb, binGroupName, true, cfg.getRateCorr, cfg.getHistBinCorr);

    // Log and mark bins as processed
    std::cout << printTimestamp() << " -- Bin group " << binGroupName
              << " contains ";
    for (const auto &bin : binCmb.cp().bin_set()) {
      std::cout << bin << ", ";
      processedBins.insert(std::move(bin));
    }
    std::cout << "\n" << std::endl;
  }

  // Process ungrouped bins
  for (const auto &bin : cmb.cp().bin_set()) {

    bool isBinGrouped = (processedBins.count(bin) > 0);

    // Create CombineHarvester for the current bin
    ch::CombineHarvester binCmb = cmb.cp().bin({bin});

    // Log warning if the bin has no matching processes
    if (binCmb.cp().bin_set().empty()) {
      std::cerr << "Warning: Bin '" << bin << "' has no matching processes."
                << std::endl;
      continue;
    }

    // Compute bin statistics
    computeBin(binCmb, bin, !isBinGrouped || cfg.sepBinHists,
               isBinGrouped ? cfg.sepBinRateCorr : cfg.getRateCorr,
               isBinGrouped ? cfg.sepBinHistBinCorr : cfg.getHistBinCorr);
  }

  std::cout << printTimestamp() << " Completed computing "
            << (isPostfit ? "post-fit" : "pre-fit") << " results....\n\n\n"
            << std::endl;
}

int main(int argc, char *argv[]) {

  displayStartupMessage();

  gROOT->SetBatch();
  gStyle->SetOptStat(0);
  gStyle->SetLineScalePS(1);
  gStyle->SetCanvasPreferGL(1);

  gSystem->Load("libHiggsAnalysisCombinedLimit");
  ChronoSpectraConfig cfg = parseCommandLine(argc, argv);

  // Ensure either pre-fit or post-fit histograms are requested
  if (cfg.skipprefit && !cfg.postfit)
    throw std::runtime_error(
        "At least one of skipprefit=false or postfit=true must be set.");

  // Parse group arguments
  auto binGroupsExp = parseNamedGroups(cfg.groupBinsArg);
  if (!binGroupsExp) {
    for (const auto &msg : binGroupsExp.error()) {
      std::cerr << "groupBins: " << msg << "\n";
    }
    throw std::runtime_error("Failed to parse --groupBins option");
  }
  auto binGroups = std::move(binGroupsExp.value());

  auto processGroupsExp = parseNamedGroups(cfg.groupProcsArg);
  if (!processGroupsExp) {
    for (const auto &msg : processGroupsExp.error()) {
      std::cerr << "groupProcs: " << msg << "\n";
    }
    throw std::runtime_error("Failed to parse --groupProcs option");
  }
  auto processGroups = std::move(processGroupsExp.value());

  std::unique_ptr<RooFitResult> fitResPtr;
  RooFitResult *fitRes = nullptr;
  if (cfg.postfit) {
    // Load RooFitResult safely
    try {
      fitResPtr = std::make_unique<RooFitResult>(
          ch::OpenFromTFile<RooFitResult>(cfg.fitresult));
    } catch (const std::exception &e) {
      throw std::runtime_error("Failed to load RooFitResult: " +
                               std::string(e.what()));
    }

    fitRes = (fitResPtr && fitResPtr->floatParsFinal().getSize() > 0)
                 ? fitResPtr.get()
                 : nullptr;

    if (fitRes) {
      std::cout << printTimestamp() << " Valid fit result found (" << cfg.fitresult
                << "), with " << fitRes->floatParsFinal().getSize()
                << " parameters." << std::endl;
    } else {
      throw std::runtime_error("Fit result is invalid!");
    }
  }

  std::cout << std::endl;

  // Load datacard for later histogram rebinning
  if (!std::filesystem::exists(cfg.datacard))
    throw std::runtime_error("Error: Datacard file '" + cfg.datacard +
                             "' does not exist.");
  ch::CombineHarvester cmb_restore;
  cmb_restore.SetFlag("workspaces-use-clone", true);
  cmb_restore.ParseDatacard(cfg.datacard, "", "", "", 0, "125.");
  if (cmb_restore.cp().bin_set().empty() ||
      cmb_restore.cp().process_set().empty()) {
    throw std::runtime_error(
        "Failed to load datacard '" + cfg.datacard +
        "' into cmb_restore: No bins or processes were found.");
  }
  std::cout << "\n\n"
            << printTimestamp()
            << " Successfully loaded text datacard: " << cfg.datacard << "\n"
            << std::endl;

  // Load workspace
  auto infile = std::unique_ptr<TFile>(TFile::Open(cfg.workspace.c_str()));
  if (!infile || infile->IsZombie())
    throw std::runtime_error("Failed to open workspace file: " + cfg.workspace);
  auto ws = std::unique_ptr<RooWorkspace>(
      dynamic_cast<RooWorkspace *>(infile->Get("w")));
  if (!ws)
    throw std::runtime_error("Workspace 'w' not found in file: " + cfg.workspace);
  else
    std::cout << printTimestamp() << " Loaded workspace from " << cfg.workspace
              << "\n"
              << std::endl;

  // Initialize CombineHarvester from Workspace
  ch::CombineHarvester cmb;
  cmb.SetFlag("workspaces-use-clone", true);
  ch::ParseCombineWorkspace(cmb, *ws, "ModelConfig", cfg.dataset, false);
  std::cout << "\n\n"
            << printTimestamp()
            << " Initialized CombineHarvester instance from workspace " << "\n"
            << std::endl;

  // Lambda to freeze parameters
  auto freeze_parameters = [&]() {
    if (cfg.freeze_arg.empty())
      return;
    std::vector<std::string> freezeVars;
    boost::split(freezeVars, cfg.freeze_arg, boost::is_any_of(","));

    for (const auto &item : freezeVars) {
      std::vector<std::string> parts;
      boost::split(parts, item, boost::is_any_of("="));

      ch::Parameter *par = cmb.GetParameter(parts[0]);

      if (!par)
        throw std::runtime_error("Parameter not found: " + parts[0]);

      // Set value if specified
      if (parts.size() == 2)
        par->set_val(boost::lexical_cast<double>(parts[1]));

      par->set_frozen(true);

      std::cout << "\n"
                << printTimestamp() << " Freezing parameter: " << parts[0]
                << (parts.size() == 2 ? " to " + parts[1] : "") << std::endl;
    }
    // }
  };

  // Freeze parameters if specified
  freeze_parameters();

  // Create output ROOT file
  auto outfile = std::unique_ptr<TFile>(
      TFile::Open(cfg.output.c_str(), "RECREATE"));
  if (!outfile || outfile->IsZombie())
    throw std::runtime_error("Failed to create output file: " + cfg.output);
  TH1::AddDirectory(false);

  if (cfg.storeSyst && (cfg.plotSystAll || !cfg.plotSystPatterns.empty()) &&
      !cfg.systSaveDir.empty()) {
    gSystem->MakeDirectory(cfg.systSaveDir.c_str());

    // Verify the directory exists after creation
    if (gSystem->AccessPathName(cfg.systSaveDir.c_str())) {
      throw std::runtime_error(
          "Failed to create systematics plotting directory: " +
          cfg.systSaveDir);
    }

    std::cout << "\n"
              << printTimestamp()
              << " Created systematics plotting directory: " << cfg.systSaveDir
              << std::endl;
  }

  // Generate pre-fit histograms if requested
  if (!cfg.skipprefit) {
    std::map<std::string, std::map<std::string, TH1F>> prefitHists;
    processAll(cmb, prefitHists, binGroups, processGroups, cfg, 0, nullptr,
               nullptr, nullptr, cfg.storeSyst ? outfile.get() : nullptr);
    writeHistogramsToFile(prefitHists, *outfile, "prefit");
  }

  std::cout << "\n\n";

  // Generate post-fit histograms if requested
  if (cfg.postfit) {

    // Update model parameters to post-fit values
    cmb.UpdateParameters(*fitRes);

    // // Freeze parameters if specified
    // freeze_parameters();

    // Prepare containers
    std::map<std::string, std::map<std::string, TH1F>> postfitHists;
    std::map<std::string, std::map<std::string, TH2F>> RateCorrMap,
        HistBinCorrMap;

    // Process all post-fit histograms and correlations
    processAll(cmb, postfitHists, binGroups, processGroups, cfg, cfg.samples,
               fitRes, &RateCorrMap, &HistBinCorrMap, nullptr);

    // Write histograms and correlation matrices to file
    writeHistogramsToFile(postfitHists, *outfile, "postfit");
    writeCorrToFile(RateCorrMap, *outfile, "postfit", "_RateCorr");
    writeCorrToFile(HistBinCorrMap, *outfile, "postfit", "_HistBinCorr");

    // Generate and populate parameter correlation matrix
    const RooArgList *paramList = &fitRes->floatParsFinal();
    const unsigned nPar = paramList->getSize();
    TH2F parCorrMatrix("ParCorrMat", "Parameter Correlation Matrix", nPar, 0.5,
                       nPar + 0.5, nPar, 0.5, nPar + 0.5);

    for (unsigned i = 0; i < nPar; ++i) {
      const char *paramName = (*paramList)[i].GetName();
      parCorrMatrix.GetXaxis()->SetBinLabel(i + 1, paramName);
      parCorrMatrix.GetYaxis()->SetBinLabel(i + 1, paramName);

      for (unsigned j = i; j < nPar; ++j) {
        const double corrValue = fitRes->correlationMatrix()(i, j);
        parCorrMatrix.SetBinContent(i + 1, j + 1, corrValue);
        if (i != j)
          parCorrMatrix.SetBinContent(j + 1, i + 1, corrValue);
      }
    }

    ApplyTH2FStyle(parCorrMatrix);
    ch::WriteToTFile(&parCorrMatrix, outfile.get(), "postfit/parCorrMat");
    std::cout << "\n"
              << printTimestamp()
              << " Parameter correlations extracted -> postfit/parCorrMat"
              << std::endl;

    // Compute and write global rate correlation matrix
    if (cfg.samples > 0) {
      TH2F globalRateCorrMatrix =
          cmb.cp().GetRateCorrelation(*fitRes, cfg.samples);
      ApplyTH2FStyle(globalRateCorrMatrix);
      ch::WriteToTFile(&globalRateCorrMatrix, outfile.get(),
                       "postfit/globalRateCorr");
      std::cout
          << printTimestamp() << std::setw(50) << std::left
          << " Global rate correlations computed -> postfit/globalRateCorr"
          << std::endl;
    }
  }

  // Cleanup handled by smart pointers
  std::cout << "\n\n"
            << printTimestamp() << " Output file: " << outfile->GetName()
            << std::endl;
  std::cout << "\n\n\n\n"
            << printTimestamp() << " Task complete!\n\n\n\n"
            << std::endl;
  return 0;
}
