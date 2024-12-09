/**
 ===========================================================================
 ChronoSpectra.cpp
 Author: Mohammad Abrar Wadud, 2024
 ============================================================================
 Efficient Pre-fit & Post-fit Histogram Extraction for CMS Physics Analyses
 ============================================================================
 ChronoSpectra License

 Copyright (c) 2024 Mohammad Abrar Wadud

 Permission is hereby granted to any person obtaining a copy of this software
 (the "Software"), to use the Software for personal, academic, or commercial
 purposes without restriction, including  the rights to execute, and
 distribute the Software, subject to the following conditions:

 1. The Software shall not be modified, altered, or used to create derivative
    works without the explicit written permission of the copyright holder.

 2. The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

 3. This Software is provided "as is," without warranty of any kind, express
    or implied, including but not limited to the warranties of merchantability,
    fitness for a particular purpose, and non-infringement. In no event shall
    the authors or copyright holders be liable for any claim, damages, or other
    liability, whether in an action of contract, tort, or otherwise, arising
    from, out of, or in connection with the Software or the use or other dealings
    in the Software.
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
    cmsrel CMSSW_14_1_0_pre4
    cd CMSSW_14_1_0_pre4/src
    cmsenv
    git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit
    cd $CMSSW_BASE/src/HiggsAnalysis/CombinedLimit
    git fetch origin
    git checkout v10.0.2
    cd $CMSSW_BASE/src/
    git clone https://github.com/TheQuantiser/CombineHarvester.git
    scram b
    cmsenv

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
    - Computes correlation matrices for histogram bin-to-bin and process-to-process rates.

 5. Parameter Freezing
    - Allows fixing parameters with optional values for custom fits.

 6. Organized Output
    - Saves histograms and matrices in structured directories: `prefit/`, `postfit/`.
    - Provides flexibility for grouped or individual histogram handling.

 ---------------------
 Command-Line Options:
 ---------------------
 --help / -h              : Display help information (implicit: true; default: false).
                             No input required for `true`.
 --workspace (REQUIRED)   : Input ROOT workspace file.
 --datacard (REQUIRED)    : Input datacard file for rebinning.
 --output (REQUIRED)      : Output ROOT file for storing results.
 --dataset                : Dataset name in the workspace (default: `data_obs`).
 --fitresult              : Path to RooFitResult file (default: none).
                             Format: `filename:fit_name`.
 --postfit                : Enable generation of post-fit histograms
                             (implicit: true; default: false).
                             No input required for `true`. Requires a fit result file.
 --skipprefit             : Skip generation of pre-fit histograms
                             (implicit: true; default: false).
                             No input required for `true`. At least one of
                             `--postfit` or `!skipprefit` must be enabled.
 --samples                : Number of samples for uncertainty estimation (default: 0).
 --freeze                 : Freeze parameters during the fit (default: none).
                             Example format: `PARAM1,PARAM2=X`.
 --groupBins              : Group bins under named groups (default: none).
                             Format: `group1:bin1,bin2;group2:bin3`.
 --groupProcs             : Group processes under named groups (default: none).
                             Format: `group1:proc1,proc2;group2:proc3`.
 --skipObs                : Do not generate data (observed) histograms
                             (implicit: true; default: false).
                             No input required for `true`.
 --getRateCorr            : Compute rate correlation matrices
                             (implicit: true; default: true).
                             No input required for `true`.
 --getHistBinCorr         : Compute histogram bin correlation matrices
                             (implicit: true; default: true).
                             No input required for `true`.
 --sepProcHists           : Generate separate histograms for grouped processes
                             (implicit: true; default: false).
                             No input required for `true`.
 --sepBinHists            : Generate separate histograms for grouped bins
                             (implicit: true; default: false).
                             No input required for `true`.
 --sepProcHistBinCorr     : Compute separate histogram bin correlations for grouped processes
                             (implicit: true; default: false).
                             No input required for `true`.
 --sepBinHistBinCorr      : Compute separate histogram bin correlations for grouped bins
                             (implicit: true; default: false).
                             No input required for `true`.
 --sepBinRateCorr         : Compute separate rate correlations for grouped bins
                             (implicit: true; default: false).
                            No input required for `true`.

 --------------
 Example Usage:
 --------------
ChronoSpectra --help \
--workspace workspace.root --datacard datacard.txt --output output.root \
--dataset data_obs --postfit --fitresult=fit.root:fit_mdf --samples 2000 \
--freeze Wrate=1.5,pdf --groupBins "region1:bin1,bin2;region2:bin3,bin4" \
--groupProcs "signal:procA,procB;background:procC,procD" --skipObs \
--getRateCorr=false --getHistBinCorr --skipprefit \
--sepProcHists --sepBinHists --sepProcHistBinCorr --sepBinHistBinCorr --sepBinRateCorr

 Output Structure:
 -----------------
 A root file specified by `--output` containing:
 1. Histograms: Stored under `prefit/` and `postfit/` directories.
 2. Correlation Matrices: Organized as `<bin>/<process>_HistBinCorr` or `_RateCorr`.

 Notes:
 ------
 - Ensure workspace and datacard files align to avoid mismatches.
 - Use `--samples` for sampling-based uncertainty estimation.
 - Results are saved in a structured and organized ROOT file.
**/

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <TSystem.h>
#include <TH2F.h>
#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/ParseCombineWorkspace.h"
#include "CombineHarvester/CombineTools/interface/TFileIO.h"

// User input parser
boost::program_options::options_description config("Configuration");

// Command-line arguments
std::string datacard, workspace, fitresult, output, groupBinsArg, groupProcsArg, freeze_arg, dataset = "data_obs";

// User input parameter storage
unsigned samples = 2000;
bool postfit = false, skipprefit = false;
bool skipObs = false;
bool getRateCorr = false, getHistBinCorr = false;
bool sepProcHists = false, sepBinHists = false;
bool sepProcHistBinCorr = false, sepBinHistBinCorr = false;
bool sepBinRateCorr = false;
ch::CombineHarvester* cmb_restore_ptr = nullptr;


std::string printTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&time_t_now);

    std::ostringstream timestamp;
    timestamp << "[" << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S") << "]";
    return timestamp.str();
}

// Function: parseNamedGroups
// --------------------------
// Parses named groups of bins or processes from the command-line argument format.
// Example format: "group1:bin1,bin2;group2:bin3,bin4"
// Input:
// - groupsArg: A string containing semicolon-separated group definitions with names.
// Output:
// - A map where keys are group names, and values are vectors of items in the group.
std::map<std::string, std::vector<std::string>> parseNamedGroups(const std::string &groupsArg) {
    std::map<std::string, std::vector<std::string>> namedGroups;
    if (groupsArg.empty()) return namedGroups;
    std::vector<std::string> groupStrings;
    boost::split(groupStrings, groupsArg, boost::is_any_of(";"));
    for (auto &group : groupStrings) {
        boost::trim(group); // Trim spaces around each group
        size_t colonPos = group.find(':');
        if (colonPos == std::string::npos || colonPos == 0 || colonPos == group.size() - 1 ||
                group.find(':', colonPos + 1) != std::string::npos) {
            std::cerr << "Warning: Skipping invalid group: '" << group << "'" << std::endl;
            continue;
        }
        std::string groupName = boost::trim_copy(group.substr(0, colonPos));
        if (groupName.empty() || groupName.find(' ') != std::string::npos) {
            std::cerr << "Warning: Skipping group with invalid name: '" << groupName << "'" << std::endl;
            continue;
        }
        if (!namedGroups.emplace(groupName, std::vector<std::string>()).second) {
            std::cerr << "Warning: Duplicate group name found: '" << groupName << "'" << std::endl;
            continue;
        }
        std::vector<std::string> items;
        boost::split(items, group.substr(colonPos + 1), boost::is_any_of(","));
        for (auto &item : items) {
            boost::trim(item); // Trim spaces around each item
        }
        if (items.empty() || std::any_of(items.begin(), items.end(), [](const std::string & item) {
        return item.empty() || item.find(' ') != std::string::npos;
        })) {
            std::cerr << "Warning: Skipping group with invalid items: '" << group << "'" << std::endl;
            namedGroups.erase(groupName);
            continue;
        }
        namedGroups[groupName] = std::move(items);
    }
    // Print out the groups and their elements in a neatly formatted way
    for (const auto &group : namedGroups) {
        std::cout << "Group: '" << group.first << "' -> \t: '";
        for (size_t i = 0; i < group.second.size(); ++i) {
            std::cout << group.second[i];
            if (i != group.second.size() - 1) std::cout << "', ";
        }
        std::cout << "\n";
    }
    return namedGroups;
}

// Function: writeHistogramsToFile
// -------------------------------
// Writes a collection of histograms to a specified ROOT output file, organizing them into directories
// based on a given prefix and bin/process structure.
//
// Parameters:
// - histograms:
//   - Type: std::map<std::string, std::map<std::string, TH1F>>.
//   - Description: A nested map containing histograms grouped by bins and processes.
//     - Outer key: Bin name.
//     - Inner key: Process name.
//     - Value: A TH1F object representing the histogram for the given bin and process.
//
// - outfile:
//   - Type: TFile&.
//   - Description: Reference to the output ROOT file where the histograms will be saved.
//     The file must already be open.
//
// - prefix:
//   - Type: const std::string&.
//   - Description: Prefix for the directory path in the output file (e.g., "prefit" or "postfit").
//
// Functionality:
// - Iterates through the nested map of `histograms`.
// - Constructs a directory path for each histogram using the format: `<prefix>/<binName>/<procName>`.
// - Writes each histogram to the `outfile` under its respective directory path.
// - Logs progress and completion status to the console for user feedback.
void writeHistogramsToFile(std::map<std::string, std::map<std::string, TH1F>> &histograms,
                           TFile &outfile,
                           const std::string &prefix) {
    std::cout << printTimestamp() << " Writing histograms to file: " << outfile.GetName() << std::endl;

    // Iterate over bins
    for (auto &binPair : histograms) {
        const std::string &binName = binPair.first;

        // Iterate over processes within the current bin
        for (auto &procPair : binPair.second) {
            const std::string &procName = procPair.first;
            TH1F &histogram = procPair.second;

            // Construct the directory path and write the histogram
            std::string path = prefix + "/" + binName + "/" + procName;
            std::cout << printTimestamp() << "\t- Writing to -> " << path << std::endl;
            ch::WriteToTFile(&histogram, &outfile, path);
        }
    }

    std::cout << printTimestamp() << " ... done." << std::endl;
}

// Function: writeCorrToFile
// -------------------------
// This function writes correlation matrices to a specified ROOT output file. The matrices are organized
// into a directory structure based on provided prefixes and suffixes.
//
// Parameters:
// - matrixMap:
//   - Type: std::map<std::string, std::map<std::string, TH2F>>.
//   - Description: A nested map containing correlation matrices grouped by bin and process.
//     - Outer key: Bin name.
//     - Inner key: Process name.
//     - Value: A TH2F object representing the correlation matrix for the given bin and process.
//
// - outfile:
//   - Type: TFile&.
//   - Description: Reference to the output ROOT file where the correlation matrices will be written.
//     The file must already be open.
//
// - prefix:
//   - Type: const std::string&.
//   - Description: Prefix for the directory path in the output file. For example, "prefit" or "postfit".
//
// - suffix:
//   - Type: const std::string&.
//   - Description: Suffix appended to the directory path for the correlation matrix. For example,
//     "_RateCorr" or "_HistBinCorr".
//
// Functionality:
// - Iterates through the nested structure of `matrixMap` to retrieve correlation matrices for each bin and process.
// - For each matrix, it constructs a directory path using the provided `prefix` and `suffix` in the form:
//   `<prefix>/<binName>/<procName><suffix>`.
// - Writes each matrix to the `outfile` under its respective directory path.
//
// Logging:
// - Logs the file name and the directory paths for each written matrix to the console.
// - Logs the completion of the operation.
//
// Example Usage:
// ```cpp
// std::map<std::string, std::map<std::string, TH2F>> myMatrixMap;
// TFile outfile("output.root", "RECREATE");
// writeCorrToFile(myMatrixMap, outfile, "postfit", "_RateCorr");
// ```
//
// Notes:
// - Ensure the `outfile` is open and writable before calling this function.
// - The function does not modify the `matrixMap` or the matrices it contains.
// - The directory structure in the output ROOT file is created automatically.
void writeCorrToFile(std::map<std::string, std::map<std::string, TH2F>> &matrixMap,
                     TFile &outfile,
                     const std::string &prefix,
                     const std::string &suffix) {
    // Log the start of the writing process
    std::cout << "\n" << printTimestamp() << " Writing correlation matrices to file: " << outfile.GetName() << std::endl;

    // Iterate over bins in the matrix map
    for (auto &binPair : matrixMap) {
        const std::string &binName = binPair.first;

        // Iterate over processes within each bin
        for (auto &procPair : binPair.second) {
            const std::string &procName = procPair.first;
            TH2F &matrix = procPair.second;

            // Construct the directory path
            const std::string dir = prefix + "/" + binName + "/" + procName + suffix;

            // Log the path being written
            std::cout << printTimestamp() << "\t- Writing to -> " << dir << std::endl;

            // Set draw options and write the matrix to the output file
            matrix.SetOption("colz");
            matrix.SetDrawOption("colz");
            matrix.GetXaxis()->LabelsOption("v");
            matrix.GetZaxis()->SetMoreLogLabels();
            ch::WriteToTFile(&matrix, &outfile, dir);
        }
    }

    // Log completion of the writing process
    std::cout << printTimestamp() << " ... done." << std::endl;
}


// Function: processHistograms
// ---------------------------
// Extracts histograms for processes within a CombineHarvester instance.
// Input:
// - cmb: The CombineHarvester instance to process.
// - histograms: A map to store extracted histograms for each process group.
// - processGroups: Named groupings of processes.
// - samples: Number of samples for uncertainty estimation (0 disables sampling).
// - fitRes: Pointer to RooFitResult for post-fit histograms (nullptr for pre-fit).
// - RateCorrMap: (Optional) Map to store covariance matrices for each process group.
// - HistBinCorrMap: (Optional) Map to store correlation matrices for each process group.
void processHistograms(ch::CombineHarvester &cmb,
                       std::map<std::string, TH1F> &histograms,
                       const std::map<std::string, std::vector<std::string>> &processGroups,
                       unsigned samples, RooFitResult *fitRes,
                       std::map<std::string, TH2F> *RateCorrMap = nullptr,
                       std::map<std::string, TH2F> *HistBinCorrMap = nullptr) {

    bool doSamplingUnc = (samples > 0) && (fitRes != nullptr);

    histograms["total"] = doSamplingUnc ?  cmb.cp().GetShapeWithUncertainty(*fitRes, samples) :
                          cmb.cp().GetShapeWithUncertainty();
    if (skipObs) {
        // Copy the content of total processes histogram when skipObs is true
        TH1F pseudoData = histograms["total"];  // Copy construct a new histogram object
        pseudoData.SetName(dataset.c_str());
        histograms[dataset] = pseudoData;          // Assign the copied object
        std::cout << printTimestamp() << " Observed (pseudo data): " << histograms[dataset].Integral() << " +- " << std::sqrt(histograms[dataset].Integral()) << std::endl;
    } else {
        histograms[dataset] = cmb.cp().GetObservedShape();
        std::cout << printTimestamp() << " Observed: " << histograms[dataset].Integral() << " +- " << std::sqrt(histograms[dataset].Integral()) << std::endl;
    }
    histograms[dataset].SetBinErrorOption(TH1::kPoisson);

    std::cout << printTimestamp() << " Sum of all modeled processes: " << histograms["total"].Integral() << " +- " << histograms["total"].GetBinContent(0) << std::endl;

    histograms["background"] = doSamplingUnc ?  cmb.cp().backgrounds().GetShapeWithUncertainty(*fitRes, samples) :
                               cmb.cp().backgrounds().GetShapeWithUncertainty();
    std::cout << printTimestamp() << " Total background: " << histograms["background"].Integral() << " +- " << histograms["background"].GetBinContent(0) << std::endl;
    histograms["signal"] = doSamplingUnc ?  cmb.cp().signals().GetShapeWithUncertainty(*fitRes, samples) :
                           cmb.cp().signals().GetShapeWithUncertainty();
    std::cout << printTimestamp() << " Total signal: " << histograms["signal"].Integral() << " +- " << histograms["signal"].GetBinContent(0) << std::endl;

    std::unordered_set<std::string> processedProcesses;
    // Process specified groups
    for (const auto &group : processGroups) {
        const std::string &groupName = group.first;
        const std::vector<std::string> &processList = group.second;
        std::cout << printTimestamp() << " Processing process group: " << groupName ;
        ch::CombineHarvester groupCmb = cmb.cp().process_rgx(processList);
        // Skip groups with no matching processes
        if (groupCmb.cp().process_set().empty()) {
            std::cerr << "\nWarning: Process group '" << groupName << "' has no matching processes." << std::endl;
            continue;
        }
        // Generate histograms for the process group
        histograms[groupName] = doSamplingUnc
                                ? groupCmb.cp().GetShapeWithUncertainty(*fitRes, samples)
                                : groupCmb.cp().GetShapeWithUncertainty();
        // Restore the binning to match the reference histogram
        std::cout << "...  " << histograms[groupName].Integral() << " +- " << histograms[groupName].GetBinContent(0) << std::endl ;
        // Log matching processes and mark them as processed
        std::cout << printTimestamp() << " ---- Processes included in process group '" << groupName << "': ";
        for (const auto &proc : groupCmb.cp().process_set()) {
            std::cout << proc << ", ";
            processedProcesses.insert(proc);
        }
        std::cout << "\n";
        // Compute correlation matrices, if requested
        if (fitRes && postfit && samples > 0) {
            // if (RateCorrMap && getRateCorr) {
            //     (*RateCorrMap)[groupName] = groupCmb.cp().GetRateCorrelation(*fitRes, samples);
            //     std::cout << printTimestamp() << " Covariance matrix computed for process group: " << groupName << std::endl;
            // }
            if (HistBinCorrMap && getHistBinCorr) {
                (*HistBinCorrMap)[groupName] = groupCmb.cp().GetHistogramBinCorrelation(*fitRes, samples);
                std::cout << printTimestamp() << " ---- Histogram bin correlation matrix computed for process group: " << groupName << std::endl;
            }
        }
    }
    // Process ungrouped individual processes
    for (const auto &proc : cmb.cp().process_set()) {
        bool processIsGrouped = processedProcesses.find(proc) != processedProcesses.end();
        bool doProcHists = !processIsGrouped || sepProcHists;
        bool doProcessHistBinCorr = sepProcHistBinCorr || !processIsGrouped;
        if (!doProcHists && !doProcessHistBinCorr) continue;
        std::cout << printTimestamp() << " Processing ungrouped process: " << proc;
        ch::CombineHarvester singleProcCmb = cmb.cp().process({proc});
        // Skip pronces for no match
        if (singleProcCmb.cp().process_set().empty()) {
            std::cerr << "\nWarning: process '" << proc << "' not found." << std::endl;
            continue;
        }
        // Generate histograms for the individual process
        histograms[proc] = doSamplingUnc
                           ? singleProcCmb.cp().GetShapeWithUncertainty(*fitRes, samples)
                           : singleProcCmb.cp().GetShapeWithUncertainty();
        // Restore the binning to match the reference histogram

        std::cout << "...  " << histograms[proc].Integral() << " +- " << histograms[proc].GetBinContent(0) << std::endl ;
        // Compute correlation matrices, if requested
        if (doSamplingUnc) {
            if (doProcessHistBinCorr && HistBinCorrMap && getHistBinCorr) {
                (*HistBinCorrMap)[proc] = singleProcCmb.cp().GetHistogramBinCorrelation(*fitRes, samples);
                std::cout << printTimestamp() << " ---- Histogram bin correlation matrix computed for individual process: " << proc << std::endl;
            }
        }
    }

    if (cmb_restore_ptr != nullptr && !cmb.cp().bin_set().empty()) {
        TH1F refHist = cmb_restore_ptr->cp().bin({ * (cmb.cp().bin_set().begin()) }).GetObservedShape();
        for (auto & histPair : histograms) {
            histPair.second = ch::RestoreBinning(histPair.second, refHist);
        }
    }
}

// Function: processAll
// --------------------
// Processes all bins and their associated histograms.
// Input           :
// - cmb           : The main CombineHarvester instance.
// - histograms    : A map to store processed histograms for each bin and process.
// - binGroups     : Named groupings of bins.
// - processGroups : Named groupings of processes.
// - samples       : Number of samples for uncertainty estimation (0 disables sampling).
// - fitRes        : Pointer to RooFitResult for post-fit histograms (nullptr for pre-fit).
// - RateCorrMap        : (Optional) Map to store rate correlation matrices for each bin group.
// - HistBinCorrMap       : (Optional) Map to store histogram bin correlation matrices for each bin group.
void processAll(ch::CombineHarvester &cmb,
                std::map<std::string, std::map<std::string, TH1F>> &histograms,
                const std::map<std::string, std::vector<std::string>> &binGroups,
                const std::map<std::string, std::vector<std::string>> &processGroups,
                unsigned samples = 0, RooFitResult *fitRes = nullptr,
                std::map<std::string, std::map<std::string, TH2F>> *RateCorrMap = nullptr,
                std::map<std::string, std::map<std::string, TH2F>> *HistBinCorrMap = nullptr) {
    std::unordered_set<std::string> processedBins;
    // Process named bin groups
    for (const auto &group : binGroups) {
        const std::string &groupName = group.first;
        const std::vector<std::string> &binList = group.second;
        std::cout << "\n" << printTimestamp() << " Processing bin group: " << groupName << std::endl;
        ch::CombineHarvester binCmb = cmb.cp().bin_rgx(binList);
        if (!binCmb.cp().bin_set().empty()) {
            std::map<std::string, TH1F> localHistograms;
            std::map<std::string, TH2F> localRateCorrMap, localHistBinCorrMap;
            // Process histograms for the bin group
            processHistograms(binCmb, localHistograms, processGroups, samples, fitRes,
                              RateCorrMap ? &localRateCorrMap : nullptr, HistBinCorrMap ? &localHistBinCorrMap : nullptr);
            histograms[groupName] = std::move(localHistograms);
            // Assign correlation matrices to the correct structure
            if (fitRes && samples > 0) {
                if (RateCorrMap && getRateCorr) {
                    for (const auto &entry : localRateCorrMap) {
                        (*RateCorrMap)[groupName][entry.first] = std::move(entry.second);
                    }
                    (*RateCorrMap)[groupName][groupName] = binCmb.cp().GetRateCorrelation(*fitRes, samples);
                    std::cout << printTimestamp() << " ---- Rate correlation matrix computed for bin group: " << groupName << std::endl;
                }
                if (HistBinCorrMap && getHistBinCorr) {
                    for (const auto &entry : localHistBinCorrMap) {
                        (*HistBinCorrMap)[groupName][entry.first] = std::move(entry.second);
                    }
                    (*HistBinCorrMap)[groupName][groupName] = binCmb.cp().GetHistogramBinCorrelation(*fitRes, samples);
                    std::cout << printTimestamp() << " ---- Histogram bin correlation matrix computed for bin group: " << groupName << std::endl;
                }
            }
            // Mark bins as processed
            std::cout << printTimestamp() << " ---- Bins included in bin group '" << groupName << "': ";
            for (const auto &bin : binCmb.cp().bin_set()) {
                std::cout << bin << ", ";
                processedBins.insert(bin);
            }
            std::cout << "\n" << std::endl;
        } else {
            std::cerr << "Warning: Bin group '" << groupName << "' has no matching bins." << std::endl;
        }
    }
    // Process individual bins not in any group
    for (const auto &bin : cmb.cp().bin_set()) {
        bool binIsGrouped = processedBins.find(bin) != processedBins.end();
        bool doBinHists = sepBinHists || !binIsGrouped;
        bool doBinRateCorr = getRateCorr && RateCorrMap && (sepBinRateCorr || !binIsGrouped);
        bool doBinHistBinCorr = getHistBinCorr && HistBinCorrMap && (sepBinHistBinCorr || !binIsGrouped);
        if (!doBinHists && !doBinHists && !doBinHistBinCorr && !doBinRateCorr) continue;
        std::cout << "\n" << printTimestamp() << " Processing ungrouped bin: " << bin << std::endl;
        ch::CombineHarvester binCmb = cmb.cp().bin({bin});
        if (!binCmb.cp().bin_set().empty()) {
            std::map<std::string, TH1F> localHistograms;
            std::map<std::string, TH2F> localRateCorrMap, localHistBinCorrMap;
            if (doBinHists) {
                // Process histograms for the individual bin
                processHistograms(binCmb, localHistograms, processGroups, samples, fitRes,
                                  nullptr, doBinHistBinCorr ? &localHistBinCorrMap : nullptr);
                histograms[bin] = std::move(localHistograms);
            }
            // Assign correlation matrices to the correct structure
            if (doBinRateCorr) {
                (*RateCorrMap)[bin][bin] = binCmb.cp().GetRateCorrelation(*fitRes, samples);
                std::cout << printTimestamp() << " ---- Rate correlation matrix computed for bin: " << bin << std::endl;
            }
            if (doBinHistBinCorr) {
                (*HistBinCorrMap)[bin][bin]  = binCmb.cp().GetHistogramBinCorrelation(*fitRes, samples);
                std::cout << printTimestamp() << " ---- Histogram bin correlation matrix computed for bin: " << bin << std::endl;
            }
            std::cout << printTimestamp() << " Processed bin: " << bin << "\n" << std::endl;
        } else {
            std::cerr << "Warning: Bin '" << bin << "' has no matching processes." << std::endl;
        }
    }

    std::cout << printTimestamp() << " Completed processing all bins.\n" << std::endl;
}

// Main Program
// ------------
// Parses arguments, processes histograms, and writes results to a ROOT file.
int main(int argc, char *argv[]) {
    std::cout << printTimestamp() << " Running ChronoSpectra (c) MAW 2024 " << "\n" << std::endl;
    gSystem->Load("libHiggsAnalysisCombinedLimit");
    // Define command-line options
    bool show_help = false;
    boost::program_options::options_description config("Allowed Options");
    config.add_options()
    ("help,h", boost::program_options::bool_switch(&show_help), "Display help message (implicit: true; default: false).")
    ("workspace", boost::program_options::value<std::string>(&workspace)->required(), "Input ROOT workspace file (REQUIRED).")
    ("dataset", boost::program_options::value<std::string>(&dataset)->default_value(dataset), "Dataset name in the workspace (default: `data_obs`).")
    ("datacard", boost::program_options::value<std::string>(&datacard)->required(), "Input datacard file for rebinning (REQUIRED).")
    ("output", boost::program_options::value<std::string>(&output)->required(), "Output ROOT file for storing results (REQUIRED).")
    ("fitresult", boost::program_options::value<std::string>(&fitresult)->default_value(""), "Path to RooFitResult file (default: none). Format: `filename:fit_name`.")
    ("postfit", boost::program_options::value<bool>(&postfit)->default_value(false)->implicit_value(true), "Enable generation of post-fit histograms (implicit: true; default: false). Requires a fit result file.")
    ("skipprefit", boost::program_options::value<bool>(&skipprefit)->default_value(false)->implicit_value(true), "Skip generation of pre-fit histograms (implicit: true; default: false). At least one of `--postfit` or `!skipprefit` must be enabled.")
    ("samples", boost::program_options::value<unsigned>(&samples)->default_value(samples), "Number of samples for uncertainty estimation (default: 0).")
    ("freeze", boost::program_options::value<std::string>(&freeze_arg)->default_value(freeze_arg), "Freeze parameters during the fit (default: none). Example format: `PARAM1,PARAM2=X`.")
    ("groupBins", boost::program_options::value<std::string>(&groupBinsArg)->default_value(groupBinsArg), "Group bins under named groups (default: none). Format: `group1:bin1,bin2;group2:bin3`.")
    ("groupProcs", boost::program_options::value<std::string>(&groupProcsArg)->default_value(groupProcsArg), "Group processes under named groups (default: none). Format: `group1:proc1,proc2;group2:proc3`.")
    ("skipObs", boost::program_options::value<bool>(&skipObs)->default_value(skipObs)->implicit_value(true), "Do not generate data (observed) histograms (implicit: true; default: false). Pseudo-data equal to the summed processes will be created instead. No input required for `true`.")
    ("getRateCorr", boost::program_options::value<bool>(&getRateCorr)->default_value(getRateCorr)->implicit_value(true), "Compute rate correlation matrices (implicit: true; default: true). No input required for `true`.")
    ("getHistBinCorr", boost::program_options::value<bool>(&getHistBinCorr)->default_value(getHistBinCorr)->implicit_value(true), "Compute histogram bin correlation matrices (implicit: true; default: true). No input required for `true`.")
    ("sepProcHists", boost::program_options::value<bool>(&sepProcHists)->default_value(sepProcHists)->implicit_value(true), "Generate separate histograms for grouped processes (implicit: true; default: false). No input required for `true`.")
    ("sepBinHists", boost::program_options::value<bool>(&sepBinHists)->default_value(sepBinHists)->implicit_value(true), "Generate separate histograms for grouped bins (implicit: true; default: false). No input required for `true`.")
    ("sepProcHistBinCorr", boost::program_options::value<bool>(&sepProcHistBinCorr)->default_value(sepProcHistBinCorr)->implicit_value(true), "Compute separate histogram bin correlations for grouped processes (implicit: true; default: false). No input required for `true`.")
    ("sepBinHistBinCorr", boost::program_options::value<bool>(&sepBinHistBinCorr)->default_value(sepBinHistBinCorr)->implicit_value(true), "Compute separate histogram bin correlations for grouped bins (implicit: true; default: false). No input required for `true`.")
    ("sepBinRateCorr", boost::program_options::value<bool>(&sepBinRateCorr)->default_value(sepBinRateCorr)->implicit_value(true), "Compute separate rate correlations for grouped bins (implicit: true; default: false). No input required for `true`.")
    ("Example Usage:", ""
     "ChronoSpectra --workspace workspace.root --datacard datacard.txt --output output.root --dataset data_obs --postfit --fitresult=fit.root:fit_mdf --samples 2000 --freeze Wrate=1.5,pdf --groupBins 'region1: bin1, bin2; region2: bin3, bin4' --groupProcs 'signal: procA, procB; background: procC, procD' --skipObs --getRateCorr=false --getHistBinCorr --skipprefit --sepProcHists --sepBinHists --sepProcHistBinCorr --sepBinHistBinCorr --sepBinRateCorr");
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, config), vm);
    // Check for help flag before validating required options
    if (vm.count("help") && vm["help"].as<bool>()) {
        std::cout << config << std::endl;
        return 0;
    }
    // Validate other options
    boost::program_options::notify(vm);

    // Print all parameters and their values
    std::cout << printTimestamp() << "\n\nUsing option values:" << std::endl;
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
    // Ensure either pre-fit or post-fit histograms are requested
    if (skipprefit && !postfit) throw std::runtime_error("At least one of skipprefit=false or postfit=true must be set.");
    // Parse group arguments
    auto binGroups = parseNamedGroups(groupBinsArg);
    auto processGroups = parseNamedGroups(groupProcsArg);
    // Load datacard for later histogram rebinning
    ch::CombineHarvester cmb_restore;
    cmb_restore.SetFlag("workspaces-use-clone", true);
    cmb_restore.ParseDatacard(datacard, "", "", "", 0, "125.");
    if (cmb_restore.cp().bin_set().empty() || cmb_restore.cp().process_set().empty()) {
        throw std::runtime_error("Failed to load datacard " + datacard + " into cmb_restore: No bins or processes were found.");
    } else {
        std::cout << printTimestamp() << " Loaded text datacard " << datacard << "\n" << std::endl;
    }
    cmb_restore_ptr = &cmb_restore;
    // Load workspace
    TFile infile(workspace.c_str());
    if (!infile.IsOpen()) throw std::runtime_error("Failed to open workspace file: " + workspace);
    RooWorkspace *ws = dynamic_cast<RooWorkspace *>(infile.Get("w"));
    if (!ws) throw std::runtime_error("Workspace 'w' not found in file: " + workspace);
    else  std::cout << printTimestamp() << " Loaded workspace from " << workspace << "\n" << std::endl;
    // Initialize CombineHarvester
    ch::CombineHarvester cmb;
    cmb.SetFlag("workspaces-use-clone", true);
    ch::ParseCombineWorkspace(cmb, *ws, "ModelConfig", dataset, true);
    std::cout << printTimestamp() << " Initialized CombineHarvester instance from workspace " << "\n" << std::endl;
    // Freeze parameters if specified
    if (!freeze_arg.empty()) {
        std::vector<std::string> freezeVars;
        boost::split(freezeVars, freeze_arg, boost::is_any_of(","));
        for (const auto &item : freezeVars) {
            std::vector<std::string> parts;
            boost::split(parts, item, boost::is_any_of("="));
            ch::Parameter *par = cmb.GetParameter(parts[0]);
            if (!par) throw std::runtime_error("Parameter not found: " + parts[0]);
            if (parts.size() == 2) par->set_val(boost::lexical_cast<double>(parts[1]));
            par->set_frozen(true);
            std::cout << "Freezing parameter: " << parts[0] << (parts.size() == 2 ? " to " + parts[1] : "") << std::endl;
        }
    }
    // Create output ROOT file
    TFile outfile(output.c_str(), "RECREATE");
    if (!outfile.IsOpen()) throw std::runtime_error("Failed to create output file: " + output);
    TH1::AddDirectory(false);
    // Generate pre-fit histograms if requested
    if (!skipprefit) {
        std::map<std::string, std::map<std::string, TH1F>> prefitHists;
        std::cout << "\n" << printTimestamp() << " Generating pre-fit histograms..." << std::endl;
        processAll(cmb, prefitHists, binGroups, processGroups, 0, nullptr);
        writeHistogramsToFile(prefitHists, outfile, "prefit");
    }
    // Generate post-fit histograms if requested
    if (postfit) {
        RooFitResult res = ch::OpenFromTFile<RooFitResult>(fitresult);
        cmb.UpdateParameters(res);

        std::map<std::string, std::map<std::string, TH1F>> postfitHists;
        std::map<std::string, std::map<std::string, TH2F>> RateCorrMap, HistBinCorrMap;

        std::cout << "\n" << printTimestamp() << " Generating post-fit histograms..." << std::endl;
        processAll(cmb, postfitHists, binGroups, processGroups, samples, &res, &RateCorrMap, &HistBinCorrMap);

        writeHistogramsToFile(postfitHists, outfile, "postfit");
        // Write correlation matrices
        if (getRateCorr) writeCorrToFile(RateCorrMap, outfile, "postfit", "_RateCorr");

        if (getHistBinCorr) writeCorrToFile(HistBinCorrMap, outfile, "postfit", "_HistBinCorr");

        // Parameter correlation matrix
        const RooArgList& paramList = res.floatParsFinal() ;
        unsigned nPar = paramList.getSize(); // Number of parameters
        TH2F parCorrMatrix("ParCorrMat", "Parameter Correlation Matrix",
                           nPar, 0.5, nPar + 0.5, nPar, 0.5, nPar + 0.5);
        for (unsigned i = 1; i <= nPar; ++i) {
            parCorrMatrix.GetXaxis()->SetBinLabel(i, paramList[i - 1].GetName());
            parCorrMatrix.GetYaxis()->SetBinLabel(i, paramList[i - 1].GetName());
            for (unsigned j = i; j <= nPar; ++j) {
                parCorrMatrix.SetBinContent(i, j, res.correlationMatrix()(i - 1, j - 1));
                if (i != j) parCorrMatrix.SetBinContent(j, i, res.correlationMatrix()(i - 1, j - 1));
            }
        }
        parCorrMatrix.SetOption("colz"); // Ensure "colz" draw option is applied
        parCorrMatrix.SetDrawOption("colz");
        parCorrMatrix.GetXaxis()->LabelsOption("v");
        ch::WriteToTFile(&parCorrMatrix, &outfile, "postfit/ParCorrMat");
        std::cout << printTimestamp() << " Writing Parameter Correlation Matrix " << "postfit/ParCorrMat" << " to file: " << outfile.GetName() << std::endl;

    }
    // Cleanup
    if (ws) delete ws;
    infile.Close();
    outfile.Close();
    cmb_restore_ptr = nullptr;

    std::cout << printTimestamp() << " Task complete!" << std::endl;
    return 0;
}
