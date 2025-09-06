# CombineHarvester

Full documentation: http://cms-analysis.github.io/CombineHarvester

## Continuous Integration

Continuous integration and automated checks now run on GitHub Actions. The previous Travis CI and GitLab CI configurations have been removed.

## Installation

See the [standalone installation guide](docs/StandaloneInstallation.md) for
full details.  A typical quick-start workflow is:

```
git clone https://github.com/TheQuantiser/CustomCH.git
cd CustomCH
git submodule update --init
cmake -S . -B build
cmake --build build --target install
export CH_BASE=$(pwd)
source build/setup.sh
$CH_BASE/build/bin/Example1
```

If `CMAKE_INSTALL_PREFIX` is not explicitly set, the build installs into the
active Conda environment (`$CONDA_PREFIX`) when available, or the first entry of
`CMAKE_PREFIX_PATH`.  Use `-DCMAKE_INSTALL_PREFIX=/desired/path` to override this
location.

The configure step will automatically download the
[`HiggsAnalysis/CombinedLimit`](https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit)
`main` branch source if it is not present.

If you already have `HiggsAnalysis/CombinedLimit` built and installed on your
system, configure the build with

```
cmake -S . -B build -DUSE_SYSTEM_COMBINEDLIMIT=ON \
      -DCMAKE_PREFIX_PATH=/path/to/combinedlimit
```

`find_package(HiggsAnalysisCombinedLimit)` will then locate the installation via
`CMAKE_PREFIX_PATH`.
When working from a source tree that was built using the repository `Makefile`
instructions (for example cloned into `HiggsAnalysis/CombinedLimit` and built
with `make`), point CMake to the checkout with

```
cmake -S . -B build -DUSE_SYSTEM_COMBINEDLIMIT=ON \
      -DHiggsAnalysisCombinedLimit_ROOT=/path/to/HiggsAnalysis/CombinedLimit
```

You may also use the more explicit
`-DHiggsAnalysisCombinedLimit_DIR=/path/to/combinedlimit/lib/cmake/HiggsAnalysisCombinedLimit`
form. In this mode no download occurs and the headers and library from the
existing build are used.

If a build of `HiggsAnalysis/CombinedLimit` exists in a sibling directory named
`../HiggsAnalysis/CombinedLimit`, the CMake configuration will automatically
discover and use it when `-DUSE_SYSTEM_COMBINEDLIMIT=ON` is specified.

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
