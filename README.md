# CombineHarvester

Full documentation: http://cms-analysis.github.io/CombineHarvester

## Quick start

This package requires HiggsAnalysis/CombinedLimit to be in your local CMSSW area. We follow the release recommendations of the combine developers which can be found [here](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/#setting-up-the-environment-and-installation). The CombineHarvester framework is compatible with the CMSSW 14_1_X and 11_3_X series releases. The default branch, `main`, is for developments in the 14_1_X releases, and the current recommended tag is `v3.0.0-pre1`. The `v2.1.0` tag should be used in CMSSW_11_3_X.

A new full release area can be set up and compiled in the following steps:

    cmsrel CMSSW_14_1_0_pre4
    cd CMSSW_14_1_0_pre4/src
    cmsenv
    git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit
    # IMPORTANT: Checkout the recommended tag on the link above
    git clone https://github.com/cms-analysis/CombineHarvester.git CombineHarvester
    git checkout v3.0.0-pre1
    scram b

Previously this package contained some analysis-specific subpackages. These packages can now be found [here](https://gitlab.cern.ch/cms-hcg/ch-areas). If you would like a repository for your analysis package to be created in that group, please create an issue in the CombineHarvester repository stating the desired package name and your NICE username. Note: you are not obliged to store your analysis package in this central group.

## Repository layout and `CH_BASE`

Many of the standalone utilities and examples use the environment variable
`CH_BASE` to locate input files. This variable should point to the root of the
CombineHarvester repository. If it is not set, the tools will attempt to infer
the location automatically. The expected default layout is

```
$CH_BASE/            # Clone of this repository
$CH_BASE/CombineTools/input/
$CH_BASE/../auxiliaries/     # Optional external data repository
```

In the examples the helper functions in `CombineTools/interface/PathTools.h`
provide convenient access to these locations, e.g. `ch::paths::auxiliaries()`
and `ch::paths::input()`.
