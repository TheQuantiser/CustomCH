#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <utility>
#include <set>
#include <fstream>
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/range/algorithm_ext/erase.hpp"
#include "boost/range/algorithm/find.hpp"
#include "boost/format.hpp"
#include "TDirectory.h"
#include "TH1.h"
#include "TH2.h"
#include "CombineHarvester/CombineTools/interface/Observation.h"
#include "CombineHarvester/CombineTools/interface/Process.h"
#include "CombineHarvester/CombineTools/interface/Systematic.h"
#include "CombineHarvester/CombineTools/interface/Parameter.h"
#include "CombineHarvester/CombineTools/interface/MakeUnique.h"
#include "CombineHarvester/CombineTools/interface/Utilities.h"
#include "CombineHarvester/CombineTools/interface/Algorithm.h"

// #include "TMath.h"
// #include "boost/format.hpp"
// #include "Utilities/interface/FnPredicates.h"
// #include "Math/QuantFuncMathCore.h"

namespace ch {

CombineHarvester::ProcSystMap CombineHarvester::GenerateProcSystMap() {
  ProcSystMap lookup(procs_.size());
  for (unsigned i = 0; i < systs_.size(); ++i) {
    for (unsigned j = 0; j < procs_.size(); ++j) {
      if (MatchingProcess(*(systs_[i]), *(procs_[j]))) {
        lookup[j].push_back(systs_[i].get());
      }
    }
  }
  return lookup;
}

double CombineHarvester::GetUncertainty() {
  // Generate systematic mappings for processes
  auto lookup = GenerateProcSystMap();

  // Calculate the nominal rate before modifying any parameters
  const double rate_nominal = this->GetRateInternal(lookup);

  // Initialize accumulators for split-normal variance calculation
  double variance = 0.0;

  // Iterate through all parameters and compute uncertainties
  for (const auto& param_it : params_) {
    ch::Parameter* param = param_it.second.get();
    const double backup = param->val();  // Backup original parameter value
    const double err_d = param->err_d(); // Downward adjustment
    const double err_u = param->err_u(); // Upward adjustment

    // Downward adjustment
    param->set_val(backup + err_d);
    const double rate_d = this->GetRateInternal(lookup);

    // Upward adjustment
    param->set_val(backup + err_u);
    const double rate_u = this->GetRateInternal(lookup);

    // Restore original parameter value
    param->set_val(backup);

    // Compute components of the split-normal variance
    const double sigma_1 = std::fabs(rate_nominal - rate_d); // Left-side deviation
    const double sigma_2 = std::fabs(rate_u - rate_nominal); // Right-side deviation

    // Add the split-normal variance contribution for this parameter.
    // See https://en.wikipedia.org/wiki/Split_normal_distribution
    // Some critiques here: https://www.slac.stanford.edu/econf/C030908/papers/WEMT002.pdf
    // variance += (1.0 - (2.0 / M_PI)) * std::pow(sigma_2 - sigma_1, 2) + sigma_1 * sigma_2;
    variance += (sigma_1 * sigma_1 + sigma_2 * sigma_2) / 2.;
  }

  // Return the total uncertainty as the square root of the accumulated variance
  return std::sqrt(variance);
}

double CombineHarvester::GetUncertainty(RooFitResult const* fit,
                                        unsigned n_samples) {
  return GetUncertainty(*fit, n_samples);
}

double CombineHarvester::GetUncertainty(RooFitResult const& fit,
                                        unsigned n_samples) {
  auto lookup = GenerateProcSystMap();
  double rate = GetRateInternal(lookup);
  double err_sq = 0.0;

  // Create a backup copy of the current parameter values
  auto backup = GetParameters();

  // Calling randomizePars() ensures that the RooArgList of sampled parameters
  // is already created within the RooFitResult
  RooArgList const& rands = fit.randomizePars();

  // Now create two aligned vectors of the RooRealVar parameters and the
  // corresponding ch::Parameter pointers
  int n_pars = rands.getSize();
  std::vector<RooRealVar const*> r_vec(n_pars, nullptr);
  std::vector<ch::Parameter*> p_vec(n_pars, nullptr);
  for (unsigned n = 0; n < p_vec.size(); ++n) {
    r_vec[n] = dynamic_cast<RooRealVar const*>(rands.at(n));
    p_vec[n] = GetParameter(r_vec[n]->GetName());
  }

  for (unsigned i = 0; i < n_samples; ++i) {
    // Randomise and update values
    fit.randomizePars();
    for (int n = 0; n < n_pars; ++n) {
      if (p_vec[n]) p_vec[n]->set_val(r_vec[n]->getVal());
    }

    double rand_rate = this->GetRateInternal(lookup);
    double err = rand_rate - rate;
    err_sq += (err * err);
  }
  this->UpdateParameters(backup);
  return std::sqrt(err_sq / double(n_samples));
}

TH1F CombineHarvester::GetShapeWithUncertainty() {
  // Generate systematic map
  auto lookup = GenerateProcSystMap();

  // Get the base shape and cache the number of bins
  TH1F shape = GetShape();
  const int n_bins = shape.GetNbinsX();
  const double nominal_rate = shape.Integral();

  // Initialize bin variances and rate variance
  std::vector<double> bin_variances(n_bins, 0.0);
  double rate_variance = 0.0;

  // Iterate through parameters to compute uncertainties
  TH1F shape_d;
  TH1F shape_u;
  for (const auto& param_it : params_) {
    ch::Parameter* param = param_it.second.get();
    const double err_d = param->err_d(); // Downward adjustment
    const double err_u = param->err_u(); // Upward adjustment

    // Backup original parameter value
    const double backup = param->val();

    // Compute shapes for downward and upward variations
    param->set_val(backup + err_d);
    shape_d = this->GetShapeInternal(lookup, param->name());
    const double rate_d = shape_d.Integral();

    param->set_val(backup + err_u);
    shape_u = this->GetShapeInternal(lookup, param->name());
    const double rate_u = shape_u.Integral();

    // Restore original parameter value
    param->set_val(backup);

    // Compute split-normal variance for the rate.
    // See https://en.wikipedia.org/wiki/Split_normal_distribution
    // Some critiques here: https://www.slac.stanford.edu/econf/C030908/papers/WEMT002.pdf
    const double sigma_1 = std::fabs(nominal_rate - rate_d); // Left-side deviation
    const double sigma_2 = std::fabs(rate_u - nominal_rate); // Right-side deviation
    // rate_variance += (1.0 - (2.0 / M_PI)) * std::pow(sigma_2 - sigma_1, 2) + sigma_1 * sigma_2;
    rate_variance += (sigma_1 * sigma_1 + sigma_2 * sigma_2) / 2.;

    // Update bin variances using split-normal distribution
    for (int bin_idx = 1; bin_idx <= n_bins; ++bin_idx) {
      const double bin_u = shape_u.GetBinContent(bin_idx);
      const double bin_d = shape_d.GetBinContent(bin_idx);
      const double bin_nom = shape.GetBinContent(bin_idx);

      const double bin_sigma_1 = std::fabs(bin_nom - bin_d);
      const double bin_sigma_2 = std::fabs(bin_u - bin_nom);
      // bin_variances[bin_idx - 1] += (1.0 - (2.0 / M_PI)) * std::pow(bin_sigma_2 - bin_sigma_1, 2) +
      //                               bin_sigma_1 * bin_sigma_2;
      bin_variances[bin_idx - 1] += (bin_sigma_1 * bin_sigma_1 + bin_sigma_2 * bin_sigma_2) / 2.;
    }
  }

  // Finalize bin errors and update the shape
  for (int bin_idx = 1; bin_idx <= n_bins; ++bin_idx) {
    shape.SetBinError(bin_idx, std::sqrt(bin_variances[bin_idx - 1]));
  }

  // Compute and store the total rate uncertainty in the underflow bin
  shape.SetBinContent(0, std::sqrt(rate_variance));

  return shape;
}

TH1F CombineHarvester::GetShapeWithUncertainty(RooFitResult const* fit,
    unsigned n_samples) {
  return GetShapeWithUncertainty(*fit, n_samples);
}

TH1F CombineHarvester::GetShapeWithUncertainty(RooFitResult const& fit, unsigned n_samples) {
  // Generate systematic mappings for processes
  auto lookup = GenerateProcSystMap();

  // Retrieve the nominal shape
  TH1F shape = GetShapeInternal(lookup);

  // Number of bins in the shape histogram
  const int n_bins = shape.GetNbinsX();

  // Initialize bin-level statistics
  std::vector<double> bin_sum(n_bins, 0.0);
  std::vector<double> bin_sum_sq(n_bins, 0.0);

  // Reset errors in the shape
  for (int bin_idx = 1; bin_idx <= n_bins; ++bin_idx) {
    shape.SetBinError(bin_idx, 0.0);
  }

  // Backup current parameter values for restoration after sampling
  auto backup = GetParameters();

  // Prepare randomized parameters from the fit result
  RooArgList const& rands = fit.randomizePars();
  const int n_pars = rands.getSize();

  // Align RooFit parameters with CombineHarvester parameters
  std::vector<RooRealVar const*> r_vec(n_pars, nullptr);
  std::vector<ch::Parameter*> p_vec(n_pars, nullptr);
  for (int idx = 0; idx < n_pars; ++idx) {
    r_vec[idx] = dynamic_cast<RooRealVar const*>(rands.at(idx));
    p_vec[idx] = GetParameter(r_vec[idx]->GetName());
  }

  // Variables for total rate calculations
  double sum_rates = 0.0;
  double sum_rates_sq = 0.0;

  // Main sampling loop
  TH1F rand_shape;
  for (unsigned sample_idx = 0; sample_idx < n_samples; ++sample_idx) {
    // Randomize and update parameter values
    fit.randomizePars();
    for (int idx = 0; idx < n_pars; ++idx) {
      if (p_vec[idx]) {
        p_vec[idx]->set_val(r_vec[idx]->getVal());
      }
    }

    // Retrieve the randomized shape
    rand_shape = this->GetShapeInternal(lookup);

    // Compute total rate and accumulate statistics
    double rand_rate = rand_shape.Integral();
    sum_rates += rand_rate;
    sum_rates_sq += rand_rate * rand_rate;

    // Accumulate bin-level statistics
    for (int bin_idx = 0; bin_idx < n_bins; ++bin_idx) {
      double yield = rand_shape.GetBinContent(1 + bin_idx);
      bin_sum[bin_idx] += yield;
      bin_sum_sq[bin_idx] += yield * yield;
    }
  }

  // Finalize bin uncertainties and update the shape histogram
  for (int bin_idx = 1; bin_idx <= n_bins; ++bin_idx) {
    double mean = bin_sum[bin_idx - 1] / n_samples;
    double variance = (bin_sum_sq[bin_idx - 1] / n_samples) - (mean * mean);
    shape.SetBinError(bin_idx, std::sqrt(variance));
  }

  // Calculate and set total rate uncertainty in the underflow bin
  double rate_variance = (sum_rates_sq / n_samples) - std::pow(sum_rates / n_samples, 2);
  shape.SetBinContent(0, std::sqrt(rate_variance));

  // Restore original parameter values
  this->UpdateParameters(backup);

  return shape;
}

TH2F CombineHarvester::GetRateCovariance(RooFitResult const& fit, unsigned n_samples) {
  // Determine the number of processes
  unsigned n_procs = procs_.size();
  if (n_procs == 0) {
    throw std::runtime_error("Error: No processes available for covariance calculation.");
  }

  // Prepare process CombineHarvesters and labels
  std::vector<CombineHarvester> ch_procs;
  std::vector<std::string> labels(n_procs);
  std::vector<double> nominal_rates(n_procs, 0.0);
  ch_procs.reserve(n_procs);

  for (unsigned i = 0; i < n_procs; ++i) {
    ch_procs.push_back(this->cp().bin({procs_[i]->bin()}).process({procs_[i]->process()}));
    labels[i] = procs_[i]->bin() + "," + procs_[i]->process();
    nominal_rates[i] = ch_procs[i].GetRate();
  }

  // Initialize storage for sums and sums of squares
  std::vector<double> sum(n_procs, 0.0);
  std::vector<double> sum2(n_procs, 0.0);
  std::vector<std::vector<double>> sum_covariance(n_procs, std::vector<double>(n_procs, 0.0));

  // Backup current parameters
  auto backup = GetParameters();

  // Initialize RooFit parameters for randomization
  RooArgList const& rands = fit.randomizePars();
  int n_pars = rands.getSize();
  std::vector<RooRealVar const*> r_vec(n_pars, nullptr);
  std::vector<ch::Parameter*> p_vec(n_pars, nullptr);

  for (int n = 0; n < n_pars; ++n) {
    r_vec[n] = dynamic_cast<RooRealVar const*>(rands.at(n));
    p_vec[n] = GetParameter(r_vec[n]->GetName());
  }

  // Preallocate vectors for randomized rates
  std::vector<double> randomized_rates(n_procs, 0.0);

  // Main sampling loop
  for (unsigned rnd = 0; rnd < n_samples; ++rnd) {
    // Randomize parameters and set values
    fit.randomizePars();
    for (int n = 0; n < n_pars; ++n) {
      if (p_vec[n]) {
        p_vec[n]->set_val(r_vec[n]->getVal());
      }
    }

    // Compute randomized rates for all processes
    for (unsigned i = 0; i < n_procs; ++i) {
      randomized_rates[i] = ch_procs[i].GetRate();
    }

    // Update sums and covariance sums in a single loop
    for (unsigned i = 0; i < n_procs; ++i) {
      sum[i] += randomized_rates[i];
      sum2[i] += randomized_rates[i] * randomized_rates[i];
      for (unsigned j = i; j < n_procs; ++j) {
        sum_covariance[i][j] += randomized_rates[i] * randomized_rates[j];
      }
    }
  }

  // Restore original parameter values
  this->UpdateParameters(backup);

  // Create ROOT histogram for the covariance matrix
  TH2F cov_mat("covariance", "Rate Covariance Matrix",
               n_procs, 0.5, n_procs + 0.5, n_procs, 0.5, n_procs + 0.5);
  // Normalize and compute covariance matrix
  for (unsigned i = 0; i < n_procs; ++i) {
    cov_mat.GetXaxis()->SetBinLabel(i + 1, labels[i].c_str());
    cov_mat.GetYaxis()->SetBinLabel(i + 1, labels[i].c_str());
    double mean_i = sum[i] / static_cast<double>(n_samples);
    for (unsigned j = i; j < n_procs; ++j) {
      double mean_j = sum[j] / static_cast<double>(n_samples);
      double covariance = sum_covariance[i][j] / n_samples - mean_i * mean_j;
      cov_mat.SetBinContent(i + 1, j + 1, covariance); // ROOT bins start at 1
      if (i != j) {
        cov_mat.SetBinContent(j + 1, i + 1, covariance); // Mirror to lower triangle
      }
    }
  }

  // Return the covariance matrix histogram
  cov_mat.SetOption("colz"); // Ensure "colz" draw option is applied
  cov_mat.SetDrawOption("colz");
  cov_mat.GetXaxis()->LabelsOption("v");
  cov_mat.GetZaxis()->SetMoreLogLabels();
  return cov_mat;
}

TH2F CombineHarvester::GetRateCorrelation(RooFitResult const& fit, unsigned n_samples) {
  TH2F cov = GetRateCovariance(fit, n_samples);
  TH2F corr_mat = cov;
  corr_mat.Reset();
  corr_mat.SetName("correlation");
  corr_mat.SetTitle("Rate Correlation Matrix");

  int nBins = cov.GetNbinsX();
  for (int i = 1; i <= nBins; ++i) {
    double var_i = cov.GetBinContent(i, i); // Variance of process i
    if (var_i <= 0.) continue; // Skip if variance is zero or negative

    for (int j = i; j <= nBins; ++j) { // Start from i to exploit symmetry
      double var_j = cov.GetBinContent(j, j); // Variance of process j
      if (var_j <= 0.) continue; // Skip if variance is zero or negative

      double correlation = cov.GetBinContent(i, j) / (std::sqrt(var_i) * std::sqrt(var_j));

      // Fill the symmetric entries
      corr_mat.SetBinContent(i, j, correlation); // Upper triangle
      corr_mat.SetBinContent(j, i, correlation); // Lower triangle
    }
  }

  return corr_mat;
}

TH2F CombineHarvester::GetHistogramBinCorrelation(RooFitResult const& fit, unsigned n_samples) {
  // Retrieve process-systematic map and combined nominal shape
  auto lookup = GenerateProcSystMap();
  TH1F nominalShape = this->GetShapeInternal(lookup);

  // Get number of bins in the histogram
  unsigned n_bins = nominalShape.GetNbinsX();
  if (n_bins == 0) {
    throw std::runtime_error("Error: Combined shape has no bins.");
  }

  // Storage for sums and sums of squares for variance and covariance computations
  std::vector<double> sum(n_bins, 0.0);
  std::vector<double> sum2(n_bins, 0.0);
  std::vector<std::vector<double>> sum_covariance(n_bins, std::vector<double>(n_bins, 0.0));

  // Backup current parameters for restoration later
  auto backup = GetParameters();

  // Map randomized RooFit parameters to CombineHarvester parameters
  RooArgList const& rands = fit.randomizePars();
  int n_pars = rands.getSize();
  std::vector<RooRealVar const*> r_vec(n_pars);
  std::vector<ch::Parameter*> p_vec(n_pars);
  TH1F randomizedShape;
  for (int n = 0; n < n_pars; ++n) {
    r_vec[n] = dynamic_cast<RooRealVar const*>(rands.at(n));
    p_vec[n] = GetParameter(r_vec[n]->GetName());
  }

  // Sampling loop
  for (unsigned rnd = 0; rnd < n_samples; ++rnd) {
    // Randomize parameters
    fit.randomizePars();
    for (int n = 0; n < n_pars; ++n) {
      if (p_vec[n]) {
        p_vec[n]->set_val(r_vec[n]->getVal());
      }
    }

    // Retrieve randomized shape
    randomizedShape = this->GetShapeInternal(lookup);

    for (unsigned i = 1; i <= n_bins; ++i) {
      double value_i = randomizedShape.GetBinContent(i);
      sum[i - 1] += value_i;
      sum2[i - 1] += value_i * value_i;

      for (unsigned j = i; j <= n_bins; ++j) {
        double value_j = randomizedShape.GetBinContent(j);
        sum_covariance[i - 1][j - 1] += value_i * value_j;
      }
    }
  }

  // Compute correlation matrix
  TH2F correlation_matrix("bin_correlation", "Histogram Bin Correlation Matrix",
                          n_bins, 0.5, n_bins + 0.5, n_bins, 0.5, n_bins + 0.5);
  for (unsigned i = 1; i <= n_bins; ++i) {
    double mean_i = sum[i - 1] / static_cast<double>(n_samples);
    double var_i = sum2[i - 1] / static_cast<double>(n_samples) - mean_i * mean_i;
    if (var_i <= 0.0) continue;
    double std_dev_i = std::sqrt(var_i);
    for (unsigned j = i; j <= n_bins; ++j) {
      double mean_j = sum[j - 1] / static_cast<double>(n_samples);
      double var_j = sum2[j - 1] / static_cast<double>(n_samples) - mean_j * mean_j;
      if (var_j <= 0.0) continue;
      double std_dev_j =  std::sqrt(var_j);
      double cov_ij = sum_covariance[i - 1][j - 1] / static_cast<double>(n_samples) - mean_i * mean_j;
      double correlation = cov_ij / (std_dev_i * std_dev_j);
      correlation_matrix.SetBinContent(i, j, correlation);
      if (i != j) {
        correlation_matrix.SetBinContent(j, i, correlation);
      }
    }
  }

  // Set axis labels for bins
  for (unsigned i = 1; i <= n_bins; ++i) {
    std::string label = "Bin " + std::to_string(i);
    correlation_matrix.GetXaxis()->SetBinLabel(i, label.c_str());
    correlation_matrix.GetYaxis()->SetBinLabel(i, label.c_str());
  }
  correlation_matrix.SetOption("colz");
  correlation_matrix.SetDrawOption("colz");
  correlation_matrix.GetXaxis()->LabelsOption("v");
  correlation_matrix.GetZaxis()->SetMoreLogLabels();

  // Restore original parameters
  this->UpdateParameters(backup);

  return correlation_matrix;
}

double CombineHarvester::GetRate() {
  auto lookup = GenerateProcSystMap();
  return GetRateInternal(lookup);
}

TH1F CombineHarvester::GetShape() {
  auto lookup = GenerateProcSystMap();
  return GetShapeInternal(lookup);
}

double CombineHarvester::GetRateInternal(ProcSystMap const& lookup, std::string const& single_sys) {
  // Initialize total rate to 0.0
  double rate = 0.0;

  // Lambda to apply systematics to a process rate
  auto apply_systematics = [&](double & process_rate, const std::vector<const ch::Systematic*>& systematics) {
    // Iterate over all systematics for the process
    for (const auto* sys : systematics) {
      if (sys->type() == "rateParam") {
        continue; // Skip rateParam systematics, as they don't affect the rate here
      }

      // Retrieve the associated parameter for this systematic
      const auto param_it = params_.find(sys->name());
      if (param_it == params_.end()) {
        throw std::runtime_error("Parameter " + sys->name() + " not found in CombineHarvester instance");
      }
      double x = param_it->second->val();

      // Apply systematic uncertainty to the process rate
      if (sys->asymm()) {
        // For asymmetric uncertainty: use logarithmic kappas
        process_rate *= logKappaForX(x * sys->scale(), sys->value_d(), sys->value_u());
      } else {
        // For symmetric uncertainty: scale the rate
        process_rate *= std::pow(sys->value_u(), x * sys->scale());
      }
    }
  };

  // Loop over all processes in the CombineHarvester instance
  for (size_t i = 0; i < procs_.size(); ++i) {
    double process_rate = procs_[i]->rate();  // Get the base rate for the current process

    if (single_sys.empty() || procs_[i]->pdf()) {
      // Apply all systematics for pdf or if `single_sys` is not specified
      apply_systematics(process_rate, lookup[i]);
    } else {
      // If `single_sys` is specified, apply systematics only if relevant
      const bool has_single_sys = ch::any_of(lookup[i], [&](Systematic const * sys) { return sys->name() == single_sys; });
      // Apply systematics only if the process has the specified systematic
      if (has_single_sys) apply_systematics(process_rate, lookup[i]);
    }

    // Add the process rate to the total rate
    rate += process_rate;
  }

  // Return the computed total rate
  return rate;
}

TH1F CombineHarvester::GetShapeInternal(ProcSystMap const& lookup, std::string const& single_sys) {
  // Disable ROOT's automatic directory saving/restoring to improve performance.
  TH1::AddDirectory(false);

  // Initialize cumulative shape and flag for initialization status.
  TH1F cumulative_shape;
  bool is_shape_initialized = false;

  // Pre-filter systematics based on single_sys.
  std::unordered_map<int, std::vector<const ch::Systematic*>> filtered_lookup;
  if (single_sys.empty()) {
    for (size_t i = 0; i < lookup.size(); ++i) {
      filtered_lookup[i] = lookup[i]; // Copy all systematics if single_sys is empty.
    }
  } else {
    for (size_t i = 0; i < lookup.size(); ++i) {
      for (auto* sys : lookup[i]) {
        if (sys->name() == single_sys) {
          filtered_lookup[i].push_back(sys); // Add only matching systematics.
          break; // Exit early since we only care about one match.
        }
      }
    }
  }

  // Lambda to apply rate systematics to a process rate.
  auto apply_rate_systematics = [&](double & rate, const Systematic * sys) {
    auto param_it = params_.find(sys->name());
    if (param_it == params_.end()) {
      throw std::runtime_error("Parameter " + sys->name() + " not found in CombineHarvester instance");
    }
    double param_val = param_it->second->val();

    if (sys->asymm()) {
      rate *= logKappaForX(param_val * sys->scale(), sys->value_d(), sys->value_u());
    } else {
      rate *= std::pow(sys->value_u(), param_val * sys->scale());
    }
  };

  // Lambda to apply shape systematics to a process shape histogram.
  auto apply_shape_systematics = [&](TH1F * shape, const Systematic * sys) {
    if (sys->type() == "shape" || sys->type() == "shapeN2" || sys->type() == "shapeU") {
      bool linear = sys->type() != "shapeN2";
      ShapeDiff(sys->scale(), shape, shape, sys->shape_d(), sys->shape_u(), linear);
    } else if (sys->type() == "shapeN") {
      if (sys->shape_u() && sys->shape_d()) {
        ShapeDiffShapeN(sys->scale(), shape, shape, sys->shape_d(), sys->shape_u());
      } else if (sys->data_u() && sys->data_d()) {
        ShapeDiffShapeN(sys->scale(), shape, sys->data_d(), sys->data_u());
      }
    }
  };

  // Temporary histogram for PDF-based processes (reuse to avoid multiple allocations).
  std::unique_ptr<TH1F> tmp_hist;

  // Lambda to prepare a histogram for a process.
  auto prepare_histogram = [&](const std::shared_ptr<Process>& proc, TH1F & process_shape) {
    if (proc->shape()) {
      process_shape = proc->ShapeAsTH1F();
    } else if (proc->pdf()) {
      if (!proc->observable()) {
        auto* matching_data = FindMatchingData(proc.get());
        std::string var_name = matching_data ? matching_data->get()->first()->GetName() : "CMS_th1x";
        proc->set_observable(dynamic_cast<RooRealVar*>(proc->pdf()->findServer(var_name.c_str())));
      }

      if (!tmp_hist) tmp_hist.reset(dynamic_cast<TH1F*>(proc->observable()->createHistogram("")));
      else tmp_hist->Reset();

      for (int bin = 1; bin <= tmp_hist->GetNbinsX(); ++bin) {
        proc->observable()->setVal(tmp_hist->GetBinCenter(bin));
        tmp_hist->SetBinContent(bin, tmp_hist->GetBinWidth(bin) * proc->pdf()->getVal());
      }
      process_shape = *tmp_hist;

      const auto* aspdf = dynamic_cast<RooAbsPdf const*>(proc->pdf());
      if ((!aspdf || !aspdf->selfNormalized()) && process_shape.Integral() > 0.0) {
        process_shape.Scale(1.0 / process_shape.Integral());
      }
    }
  };

  // Lambda to handle a single process: apply systematics, modify shape, and combine with cumulative shape.
  auto handle_process = [&](const std::shared_ptr<Process>& proc, int process_index) {
    double process_rate = proc->rate(); // Get the base rate for the process.
    TH1F process_shape;                // Histogram to store the shape for this process.

    prepare_histogram(proc, process_shape); // Prepare the histogram for the process.

    // Apply relevant systematics (rate and shape).
    if (!filtered_lookup[process_index].empty()) {
      for (auto* sys : filtered_lookup[process_index]) {
        if (sys->type() == "rateParam") continue; // Skip rate parameters.
        apply_rate_systematics(process_rate, sys);
        apply_shape_systematics(&process_shape, sys);
      }
    }

    // Ensure non-negative bin contents and scale by process rate.
    for (int bin = 1; bin <= process_shape.GetNbinsX(); ++bin) {
      double value = std::max(0.0, process_shape.GetBinContent(bin));
      process_shape.SetBinContent(bin, value * process_rate); // Combine scaling into this step.
    }

    // Combine the processed shape with the cumulative shape.
    if (!is_shape_initialized) {
      process_shape.Copy(cumulative_shape);
      cumulative_shape.Reset();
      is_shape_initialized = true;
    }
    cumulative_shape.Add(&process_shape);
  };

  // Process all entries.
  for (size_t i = 0; i < procs_.size(); ++i) {
    handle_process(procs_[i], i);
  }

  // Return the final cumulative shape.
  return cumulative_shape;
}

double CombineHarvester::GetObservedRate() {
  double rate = 0.0;
  for (unsigned i = 0; i < obs_.size(); ++i) {
    rate += obs_[i]->rate();
  }
  return rate;
}


TH1F CombineHarvester::GetObservedShape() {
  TH1F shape;
  bool shape_init = false;

  for (unsigned i = 0; i < obs_.size(); ++i) {
    TH1F proc_shape;
    double p_rate = obs_[i]->rate();
    if (obs_[i]->shape()) {
      proc_shape = obs_[i]->ShapeAsTH1F();
    } else if (obs_[i]->data()) {
      TH1F* tmp = dynamic_cast<TH1F*>(obs_[i]->data()->createHistogram(
                                        "", *(RooRealVar*)obs_[i]->data()->get()->first()));
      tmp->Sumw2(false);
      tmp->SetBinErrorOption(TH1::kPoisson);
      proc_shape = *tmp;
      delete tmp;
      proc_shape.Scale(1. / proc_shape.Integral());
    }
    proc_shape.Scale(p_rate);
    if (!shape_init) {
      proc_shape.Copy(shape);
      shape.Reset();
      shape_init = true;
    }
    shape.Add(&proc_shape);
  }
  return shape;
}

void CombineHarvester::ShapeDiff(double x,
                                 TH1F* target,
                                 const TH1* nom,
                                 const TH1* low,
                                 const TH1* high,
                                 bool linear) {
  // Precompute the smoothing factor
  const double fx = smoothStepFunc(x);
  const int nBins = target->GetNbinsX();

  // Iterate through bins
  for (int i = 1; i <= nBins; ++i) {
    const float h = high->GetBinContent(i);
    const float l = low->GetBinContent(i);
    const float n = nom->GetBinContent(i);
    float t = target->GetBinContent(i);

    if (linear) {
      // Compute linear interpolation
      const float deltaLin = 0.5f * x * ((h - l) + (h + l - 2.0f * n) * fx);
      target->SetBinContent(i, t + deltaLin);

    } else {
      // Use log-scale interpolation only if valid values are present
      const float logT = (t > 0.0f) ? std::log(t) : -999.0f;
      const float logH = (h > 0.0f && n > 0.0f) ? std::log(h / n) : 0.0f;
      const float logL = (l > 0.0f && n > 0.0f) ? std::log(l / n) : 0.0f;

      // Compute log-scale interpolation
      const float deltaLog = 0.5f * x * ((logH - logL) + (logH + logL) * fx);
      target->SetBinContent(i, std::exp(logT + deltaLog));
    }
  }
}


void CombineHarvester::ShapeDiff(double x,
                                 TH1F* target,
                                 const RooDataHist* nom,
                                 const RooDataHist* low,
                                 const RooDataHist* high) {
  // Precompute the smoothing function value
  const double fx = smoothStepFunc(x);

  // Precompute normalization factors
  const double norm_high = high->sumEntries();
  const double norm_low = low->sumEntries();
  const double norm_nom = nom->sumEntries();

  // Validate normalization to prevent division by zero
  if (norm_high <= 0.0 || norm_low <= 0.0 || norm_nom <= 0.0) {
    throw std::runtime_error("Error: Zero or negative normalization factor in ShapeDiff");
  }

  // Loop through bins (ROOT uses 1-based indexing)
  const int nBins = target->GetNbinsX();

  for (int i = 1; i <= nBins; ++i) {
    // Move to the corresponding RooDataHist bins
    high->get(i - 1);
    low->get(i - 1);
    nom->get(i - 1);

    // Calculate normalized bin weights
    const float h = static_cast<float>(high->weight() / norm_high);
    const float l = static_cast<float>(low->weight() / norm_low);
    const float n = static_cast<float>(nom->weight() / norm_nom);

    // Pre-compute the difference and correction terms
    const float diff = h - l;
    const float corr = (h + l - 2.0f * n) * fx;

    // Update target bin content
    target->SetBinContent(i, target->GetBinContent(i) + 0.5f * x * (diff + corr));
  }
}

void CombineHarvester::ShapeDiffShapeN(double x,
                                       TH1F* target,
                                       const TH1* nom,
                                       const TH1* low,
                                       const TH1* high) {
  (void)nom;  // nominal shape currently matches target
  const double fx = smoothStepFunc(x);
  const int nBins = target->GetNbinsX();
  for (int i = 1; i <= nBins; ++i) {
    const double h = high->GetBinContent(i);
    const double l = low->GetBinContent(i);
    double t = target->GetBinContent(i);
    if (t <= 0.0) {
      target->SetBinContent(i, 0.0);
      continue;
    }
    const double logT = std::log(t);
    const double logH = (h > 0.0) ? std::log(h) : logT;
    const double logL = (l > 0.0) ? std::log(l) : logT;
    const double deltaLog = 0.5 * x *
                             ((logH - logL) + (logH + logL - 2.0 * logT) * fx);
    target->SetBinContent(i, std::exp(logT + deltaLog));
  }
}

void CombineHarvester::ShapeDiffShapeN(double x,
                                       TH1F* target,
                                       const RooDataHist* low,
                                       const RooDataHist* high) {
  const double fx = smoothStepFunc(x);
  const int nBins = target->GetNbinsX();
  const double norm_low = low->sumEntries();
  const double norm_high = high->sumEntries();
  if (norm_low <= 0.0 || norm_high <= 0.0) {
    throw std::runtime_error("Error: Zero or negative normalization factor in ShapeDiffShapeN");
  }
  for (int i = 1; i <= nBins; ++i) {
    high->get(i - 1);
    low->get(i - 1);
    const double h = high->weight() / norm_high;
    const double l = low->weight() / norm_low;
    double t = target->GetBinContent(i);
    if (t <= 0.0) {
      target->SetBinContent(i, 0.0);
      continue;
    }
    const double logT = std::log(t);
    const double logH = (h > 0.0) ? std::log(h) : logT;
    const double logL = (l > 0.0) ? std::log(l) : logT;
    const double deltaLog = 0.5 * x *
                             ((logH - logL) + (logH + logL - 2.0 * logT) * fx);
    target->SetBinContent(i, std::exp(logT + deltaLog));
  }
}


// void CombineHarvester::SetParameters(std::vector<ch::Parameter> params) {
//   params_.clear();
//   for (unsigned i = 0; i < params.size(); ++i) {
//     params_[params[i].name()] = std::make_shared<ch::Parameter>(params[i]);
//   }
// }

void CombineHarvester::RenameParameter(std::string const& oldname,
                                       std::string const& newname) {
  auto it = params_.find(oldname);
  if (it != params_.end()) {
    params_[newname] = it->second;
    params_[newname]->set_name(newname);
    params_.erase(it);
  }
}

ch::Parameter const* CombineHarvester::GetParameter(
  std::string const& name) const {
  auto it = params_.find(name);
  if (it != params_.end()) {
    return it->second.get();
  } else {
    return nullptr;
  }
}

ch::Parameter* CombineHarvester::GetParameter(std::string const& name) {
  auto it = params_.find(name);
  if (it != params_.end()) {
    return it->second.get();
  } else {
    return nullptr;
  }
}

void CombineHarvester::UpdateParameters(
  std::vector<ch::Parameter> const& params) {
  for (unsigned i = 0; i < params.size(); ++i) {
    auto it = params_.find(params[i].name());
    if (it != params_.end()) {
      it->second->set_val(params[i].val());
      it->second->set_err_d(params[i].err_d());
      it->second->set_err_u(params[i].err_u());
    } else {
      if (verbosity_ >= 1) {
        LOGLINE(log(), "Parameter " + params[i].name() + " is not defined");
      }
    }
  }
}

void CombineHarvester::UpdateParameters(RooFitResult const& fit) {
  for (int i = 0; i < fit.floatParsFinal().getSize(); ++i) {
    RooRealVar const* var =
      dynamic_cast<RooRealVar const*>(fit.floatParsFinal().at(i));
    // check for failed cast here
    auto it = params_.find(std::string(var->GetName()));
    if (it != params_.end()) {
      it->second->set_val(var->getVal());
      it->second->set_err_d(var->getErrorLo());
      it->second->set_err_u(var->getErrorHi());
    } else {
      if (verbosity_ >= 1) {
        LOGLINE(log(),
                "Parameter " + std::string(var->GetName()) + " is not defined");
      }
    }
  }
}

void CombineHarvester::UpdateParameters(RooFitResult const* fit) {
  UpdateParameters(*fit);
}

std::vector<ch::Parameter> CombineHarvester::GetParameters() const {
  std::vector<ch::Parameter> params;
  for (auto const& it : params_) {
    params.push_back(*(it.second));
  }
  return params;
}

void CombineHarvester::VariableRebin(std::vector<double> bins) {
  // We need to keep a record of the Process rates before we rebin. The
  // reasoning comes from the following scenario: the user might choose a new
  // binning which excludes some of the existing bins - thus changing the
  // process normalisation. This is fine, but we also need to adjust the shape
  // Systematic entries - both the rebinning and the adjustment of the value_u
  // and value_d shifts.
  std::vector<double> prev_proc_rates(procs_.size());

  // Also hold on the scaled Process hists *after* the rebinning is done - these
  // are needed to update the associated Systematic entries
  std::vector<std::unique_ptr<TH1>> scaled_procs(procs_.size());

  for (unsigned i = 0; i < procs_.size(); ++i) {
    if (procs_[i]->shape()) {
      // Get the scaled shape here
      std::unique_ptr<TH1> copy(procs_[i]->ClonedScaledShape());
      // shape norm should only be "no_norm_rate"
      prev_proc_rates[i] = procs_[i]->no_norm_rate();
      std::unique_ptr<TH1> copy2(copy->Rebin(bins.size() - 1, "", &(bins[0])));
      // The process shape & rate will be reset here
      procs_[i]->set_shape(std::move(copy2), true);
      scaled_procs[i] = procs_[i]->ClonedScaledShape();
    }
  }
  for (unsigned i = 0; i < obs_.size(); ++i) {
    if (obs_[i]->shape()) {
      std::unique_ptr<TH1> copy(obs_[i]->ClonedScaledShape());
      std::unique_ptr<TH1> copy2(copy->Rebin(bins.size() - 1, "", &(bins[0])));
      obs_[i]->set_shape(std::move(copy2), true);
    }
  }
  for (unsigned i = 0; i < systs_.size(); ++i) {
    TH1 const* proc_hist = nullptr;
    double prev_rate = 0.;
    for (unsigned j = 0; j < procs_.size(); ++j) {
      if (MatchingProcess(*(procs_[j]), *(systs_[i].get()))) {
        proc_hist = scaled_procs[j].get();
        prev_rate = prev_proc_rates[j];
      }
    }
    if (systs_[i]->shape_u() && systs_[i]->shape_d()) {
      // These hists will be normalised to unity
      std::unique_ptr<TH1> copy_u(systs_[i]->ClonedShapeU());
      std::unique_ptr<TH1> copy_d(systs_[i]->ClonedShapeD());

      // If we found a matching Process we will scale this back up to their
      // initial rates
      if (proc_hist) {
        copy_u->Scale(systs_[i]->value_u() * prev_rate);
        copy_d->Scale(systs_[i]->value_d() * prev_rate);
      }
      std::unique_ptr<TH1> copy2_u(
        copy_u->Rebin(bins.size() - 1, "", &(bins[0])));
      std::unique_ptr<TH1> copy2_d(
        copy_d->Rebin(bins.size() - 1, "", &(bins[0])));
      // If we have proc_hist != nullptr, set_shapes will re-calculate value_u
      // and value_d for us, before scaling the new hists back to unity
      systs_[i]->set_shapes(std::move(copy2_u), std::move(copy2_d), proc_hist);
    }
  }
}

void CombineHarvester::ZeroBins(double min, double max) {
  // We need to keep a record of the Process rates before we set bins to 0. The
  // This is necessary because we need to make sure the process normalisation
  // and shape systematic entries are correctly adjusted
  std::vector<double> prev_proc_rates(procs_.size());

  // Also hold on the scaled Process hists *after* some of the bins have
  // been set to 0 - these
  // are needed to update the associated Systematic entries
  std::vector<std::unique_ptr<TH1>> scaled_procs(procs_.size());

  for (unsigned i = 0; i < procs_.size(); ++i) {
    if (procs_[i]->shape()) {
      // Get the scaled shape here
      std::unique_ptr<TH1> copy(procs_[i]->ClonedScaledShape());
      // shape norm should only be "no_norm_rate"
      prev_proc_rates[i] = procs_[i]->no_norm_rate();
      for (int j = 1; j <= copy->GetNbinsX(); j ++) {
        if (copy->GetBinLowEdge(j) >= min && copy->GetBinLowEdge(j + 1) <= max) {
          copy->SetBinContent(j, 0.);
          copy->SetBinError(j, 0.);
        }
      }
      // The process shape & rate will be reset here
      procs_[i]->set_shape(std::move(copy), true);
      scaled_procs[i] = procs_[i]->ClonedScaledShape();
    }
  }
  for (unsigned i = 0; i < obs_.size(); ++i) {
    if (obs_[i]->shape()) {
      std::unique_ptr<TH1> copy(obs_[i]->ClonedScaledShape());
      for (int j = 1; j <= copy->GetNbinsX(); j ++) {
        if (copy->GetBinLowEdge(j) >= min && copy->GetBinLowEdge(j + 1) <= max) {
          copy->SetBinContent(j, 0.);
          copy->SetBinError(j, 0.);
        }
      }
      obs_[i]->set_shape(std::move(copy), true);
    }
  }
  for (unsigned i = 0; i < systs_.size(); ++i) {
    TH1 const* proc_hist = nullptr;
    double prev_rate = 0.;
    for (unsigned j = 0; j < procs_.size(); ++j) {
      if (MatchingProcess(*(procs_[j]), *(systs_[i].get()))) {
        proc_hist = scaled_procs[j].get();
        prev_rate = prev_proc_rates[j];
      }
    }
    if (systs_[i]->shape_u() && systs_[i]->shape_d()) {
      // These hists will be normalised to unity
      std::unique_ptr<TH1> copy_u(systs_[i]->ClonedShapeU());
      std::unique_ptr<TH1> copy_d(systs_[i]->ClonedShapeD());

      // If we found a matching Process we will scale this back up to their
      // initial rates
      if (proc_hist) {
        copy_u->Scale(systs_[i]->value_u() * prev_rate);
        copy_d->Scale(systs_[i]->value_d() * prev_rate);
      }
      for (int j = 1; j <= copy_u->GetNbinsX(); j ++) {
        if (copy_u->GetBinLowEdge(j) >= min && copy_u->GetBinLowEdge(j + 1) <= max) {
          copy_u->SetBinContent(j, 0.);
          copy_u->SetBinError(j, 0.);
        }
        if (copy_d->GetBinLowEdge(j) >= min && copy_d->GetBinLowEdge(j + 1) <= max) {
          copy_d->SetBinContent(j, 0.);
          copy_d->SetBinError(j, 0.);
        }
      }
      // If we have proc_hist != nullptr, set_shapes will re-calculate value_u
      // and value_d for us, before scaling the new hists back to unity
      systs_[i]->set_shapes(std::move(copy_u), std::move(copy_d), proc_hist);
    }
  }
}


void CombineHarvester::SetPdfBins(unsigned nbins) {
  for (unsigned i = 0; i < procs_.size(); ++i) {
    std::set<std::string> binning_vars;
    if (procs_[i]->pdf()) {
      RooAbsData const* data_obj = FindMatchingData(procs_[i].get());
      std::string var_name = "CMS_th1x";
      if (data_obj) var_name = data_obj->get()->first()->GetName();
      binning_vars.insert(var_name);
    }
    for (auto & it : wspaces_) {
      for (auto & var : binning_vars) {
        RooRealVar* avar =
          dynamic_cast<RooRealVar*>(it.second->var(var.c_str()));
        if (avar) avar->setBins(nbins);
      }
    }
  }
}

// This implementation is adapted from
// HiggsAnalysis/CombinedLimit/src/ProcessNormalization.cc
// Functionality:
// This function computes a smooth logarithmic scaling factor (`logKappa(x)`)
// for a given `x` and two scaling parameters `k_low` and `k_high`.
// For |x| >= 0.5, it directly computes the power scaling.
// For |x| < 0.5, it interpolates between log(kappaHigh) and -log(kappaLow)
// using a third-order polynomial for smooth transitions. This ensures continuity
// in the first and second derivatives.
double CombineHarvester::logKappaForX(double x, double k_low, double k_high) const {
  // Handle edge cases where one of the kappa values is zero.
  // A kappa value of zero is ill-defined for scaling purposes, so return 1.0.
  if (k_high == 0.0 || k_low == 0.0) {
    if (verbosity_ >= 1) {
      LOGLINE(log(), "kappa=0.0 detected (scaling ill-defined), returning 1.0");
    }
    return 1.0;
  }

  // For |x| >= 0.5, directly compute the scaled power using kappaHigh or kappaLow.
  if (std::fabs(x) >= 0.5) {
    return (x >= 0.0 ? std::pow(k_high, x) : std::pow(k_low, -x));
  }

  // For |x| < 0.5, use smooth interpolation between log(kappaHigh) and -log(kappaLow).

  // Precompute logarithmic values for kappaHigh and kappaLow.
  double logKhi = std::log(k_high);
  double logKlo = -std::log(k_low);

  // Calculate the average (midpoint) and half-difference of log values.
  double avg = 0.5 * (logKhi + logKlo);
  double halfdiff = 0.5 * (logKhi - logKlo);

  // Compute the polynomial interpolation factor `h(2x)`.
  // The polynomial `h(2x)` is a third-order function:
  // h(2x) = (3 * (2x)^5 - 10 * (2x)^3 + 15 * (2x)) / 8.
  // It ensures smooth transitions with the following properties:
  //  - h(+/-1) = +/-1
  //  - h'(+/-1) = 0 (zero slope at +/-1)
  //  - h"(+/-1) = 0 (flat curvature at +/-1)
  double twox = 2.0 * x;
  double twox2 = twox * twox;  // Precompute (2x)^2 for efficiency.
  double alpha = 0.125 * twox * (twox2 * (3.0 * twox2 - 10.0) + 15.0);

  // Combine the average and interpolated half-difference to get logKappa(x).
  double interpolated = avg + alpha * halfdiff;

  // Return the exponential of the scaled logKappa value.
  return std::exp(interpolated * x);
}


void CombineHarvester::SetGroup(std::string const& name,
                                std::vector<std::string> const& patterns) {
  std::vector<boost::regex> rgx;
  for (auto const& pt : patterns) rgx.emplace_back(pt);
  for (auto it = params_.begin(); it != params_.end(); ++it) {
    std::string par = it->first;
    auto & groups = it->second->groups();
    if (groups.count(name)) continue;
    if (ch::contains_rgx(rgx, par)) {
      groups.insert(name);
    };
  }
}

void CombineHarvester::RemoveGroup(std::string const& name,
                                   std::vector<std::string> const& patterns) {
  std::vector<boost::regex> rgx;
  for (auto const& pt : patterns) rgx.emplace_back(pt);
  for (auto it = params_.begin(); it != params_.end(); ++it) {
    std::string par = it->first;
    auto & groups = it->second->groups();
    if (!groups.count(name)) continue;
    if (ch::contains_rgx(rgx, par)) {
      groups.erase(name);
    };
  }
}

void CombineHarvester::RenameGroup(std::string const& oldname,
                                   std::string const& newname) {
  for (auto it = params_.begin(); it != params_.end(); ++it) {
    auto & groups = it->second->groups();
    if (groups.count(oldname)) {
      groups.erase(oldname);
      groups.insert(newname);
    }
  }
}

void CombineHarvester::AddDatacardLineAtEnd(std::string const& line) {
  post_lines_.push_back(line);
}

void CombineHarvester::ClearDatacardLinesAtEnd() {
  post_lines_.clear();
}
}
