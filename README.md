# CombineHarvester

Full documentation: http://cms-analysis.github.io/CombineHarvester

## Installation

The project can be built as a standalone package with [CMake](https://cmake.org/). Clone the repository and initialise the submodules:

```
git clone https://github.com/cms-analysis/CombineHarvester.git
cd CombineHarvester
git submodule update --init --recursive
```

Configure and build the project:

```
cmake -S . -B build
cmake --build build -j4
```

Set the `CH_BASE` environment variable to the location of the repository (or installation prefix if you run `cmake --install`):

```
export CH_BASE=$(pwd)
```

Some examples require auxiliary ROOT files. These can be obtained from the [HiggsAnalysis-HiggsToTauTau-auxiliaries](https://github.com/roger-wolf/HiggsAnalysis-HiggsToTauTau-auxiliaries) repository:

```
git clone https://github.com/roger-wolf/HiggsAnalysis-HiggsToTauTau-auxiliaries.git "$CH_BASE/auxiliaries"
```

### Examples without CMSSW

After building, binaries are available in `build/bin` and can be executed directly:

```
./build/bin/Example1
python3 CombineTools/scripts/Example3.py
```

No CMSSW environment is required for these commands.

### Compatibility with CMSSW

For backward support the previous CMSSW-based workflow remains available. The framework is compatible with the CMSSW 14_1_X and 11_3_X series releases and can still be placed in a CMSSW release area together with `HiggsAnalysis/CombinedLimit` and compiled with `scram b` following the recommendations of the combine developers.

Previously this package contained some analysis-specific subpackages. These packages can now be found [here](https://gitlab.cern.ch/cms-hcg/ch-areas). If you would like a repository for your analysis package to be created in that group, please create an issue in the CombineHarvester repository stating the desired package name and your NICE username. Note: you are not obliged to store your analysis package in this central group.

## Python package

The CombineHarvester Python utilities can be installed with

```
pip install .
```

This installs the `CombineHarvester` package together with the compiled
`libCombineHarvesterCombineTools` bindings and the `pdg-round` and
`ch-maketable` command line tools.  After installing, resources shipped
with the package are accessed via `importlib.resources` so no manual path
configuration is required.

### Example

```
pdg-round 26710 177
ch-maketable limits.json table.txt
```
