# CombineHarvester

Full documentation: http://cms-analysis.github.io/CombineHarvester

## Quick start

CombineHarvester can be built as a standalone project using CMake. The
build system will automatically fetch the required
`HiggsAnalysis/CombinedLimit` dependency if it is not already present.
On networks where outbound access is blocked the dependency must be
cloned manually or provided via the `USE_SYSTEM_COMBINEDLIMIT` option.
A minimal build looks like:

```bash
git clone https://github.com/cms-analysis/CombineHarvester.git
cd CombineHarvester
cmake -S . -B build
cmake --build build -j$(nproc)
cmake --install build
```

This installs the command-line tools into the `bin` directory of the
selected prefix.

## Python package

The Python bindings can be installed in editable mode. This will invoke
the CMake build to compile the C++ libraries and copy them into the
installed package:

```bash
python -m pip install -e .
```

See [docs/StandaloneInstallation.md](docs/StandaloneInstallation.md) for
more details on dependency setup and optional Conda environments. The
current recommended tag is `v3.0.0-pre1`.

Previously this package contained some analysis-specific subpackages. These packages can now be found [here](https://gitlab.cern.ch/cms-hcg/ch-areas). If you would like a repository for your analysis package to be created in that group, please create an issue in the CombineHarvester repository stating the desired package name and your NICE username. Note: you are not obliged to store your analysis package in this central group.
