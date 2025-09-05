# CombineHarvester

Full documentation: http://cms-analysis.github.io/CombineHarvester

## Continuous Integration

Continuous integration and automated checks now run on GitHub Actions. The previous Travis CI and GitLab CI configurations have been removed.

## Installation

See the [standalone installation guide](docs/StandaloneInstallation.md) for
full details.  A typical quick-start workflow is:

```
git clone https://github.com/cms-analysis/CombineHarvester.git
cd CombineHarvester
cmake -S . -B build
cmake --build build --target install
export CH_BASE=$(pwd)
source build/setup.sh
$CH_BASE/build/bin/Example1
```

The configure step will automatically download the
[`HiggsAnalysis/CombinedLimit`](https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit)
source if it is not present.  To use a pre-installed version instead, run
`cmake` with `-DUSE_SYSTEM_COMBINEDLIMIT=ON`.

The build requires several external C++ libraries: [ROOT](https://root.cern)
(with the RooFit and RooStats components), Boost, libxml2, vdt and
HistFactory. These dependencies must be installed and discoverable by
CMake. ROOT often provides vdt and HistFactory. Set the `ROOTSYS`
environment variable to the location of your ROOT installation so that
the headers and libraries can be found:

```
export ROOTSYS=/path/to/root
```

If the packages are installed elsewhere, ensure their include and library
paths are discoverable by the compiler and linker.

Set the `CH_BASE` environment variable to the location of the repository (or installation prefix if you run `cmake --install`):

```
export CH_BASE=$(pwd)
```

Some examples require auxiliary ROOT files. These can be obtained from the
[HiggsAnalysis-HiggsToTauTau-auxiliaries](https://github.com/roger-wolf/HiggsAnalysis-HiggsToTauTau-auxiliaries)
repository. By default these files are expected in `$CH_BASE/auxiliaries/`.
This location can be overridden by setting the `CH_AUXILIARIES` environment
variable:

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

When running the python utilities such as `combineTool.py` outside of a
CMSSW release, the `--standalone` option can be used to bypass the CMSSW
environment setup in generated job scripts.

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

The Python bindings rely on the [ROOT](https://root.cern) data analysis
framework. ROOT is not distributed via PyPI and must be installed
separately before using these tools.

### Example

```
pdg-round 26710 177
ch-maketable limits.json table.txt
```
