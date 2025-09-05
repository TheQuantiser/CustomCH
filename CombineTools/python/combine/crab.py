#!/usr/bin/env python3
"""CRAB configuration used by CombineTool.

This module builds and returns a :class:`WMCore.Configuration.Configuration`
object for submitting combine jobs with CRAB.  The CRAB client is part of the
CMS software (CMSSW) distribution, therefore this module must be used from
within a CMSSW environment where :mod:`WMCore` is available.

The combine executable can be provided either via the ``COMBINE_PATH``
environment variable or passed explicitly to :func:`create_crab_config`.
If the executable cannot be located it will not be added to the list of files
sent with the job and a warning will be emitted.
"""


import os
import warnings
from pathlib import Path
from importlib import resources

try:
    from WMCore.Configuration import Configuration
except ImportError as exc:  # pragma: no cover - user environment issue
    raise ImportError(
        "WMCore is required to create CRAB configurations. "
        "Ensure that a CMSSW environment is active."
    ) from exc


def create_crab_config(combine_executable=None):
    """Create and return a CRAB :class:`Configuration` instance.

    Parameters
    ----------
    combine_executable:
        Optional path to the ``combine`` executable. If not supplied, the
        ``COMBINE_PATH`` environment variable is consulted.
    """

    combine_path = combine_executable or os.environ.get("COMBINE_PATH")
    scripts = resources.files('CombineHarvester.CombineTools.scripts')

    config = Configuration()

    config.section_('General')
    config.General.requestName = ''

    config.section_('JobType')
    config.JobType.pluginName = 'PrivateMC'
    config.JobType.psetName = str(Path('do_nothing_cfg.py'))
    config.JobType.scriptExe = ''
    config.JobType.inputFiles = [
        str(scripts / 'FrameworkJobReport.xml'),
        str(scripts / 'copyRemoteWorkspace.sh'),
    ]
    if combine_path and os.path.exists(combine_path):
        config.JobType.inputFiles.append(combine_path)
    else:
        warnings.warn(
            'combine executable not found; set COMBINE_PATH or pass the '
            'path to create_crab_config()',
            RuntimeWarning
        )
    config.JobType.outputFiles = ['combine_output.tar']

    config.section_('Data')
    config.Data.outputPrimaryDataset = 'Combine'
    config.Data.splitting = 'EventBased'
    config.Data.unitsPerJob = 1
    config.Data.totalUnits = 1
    config.Data.publication = False
    config.Data.outputDatasetTag = ''

    config.section_('User')

    config.section_('Site')
    config.Site.blacklist = ['T3_IT_Bologna', 'T3_US_UMiss']
    config.Site.storageSite = 'T2_CH_CERN'

    return config


# For backward compatibility with existing imports
config = create_crab_config()

