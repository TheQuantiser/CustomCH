from __future__ import absolute_import
import os
from pathlib import Path
from importlib import resources
from WMCore.Configuration import Configuration

# Prefer the CH_BASE environment variable to locate the combine executable,
# falling back to a relative search if it is not set.
ch_base = os.environ.get('CH_BASE')
scram_arch = os.environ.get('SCRAM_ARCH')
combine_path = os.environ.get('COMBINE_PATH')
scripts_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', 'scripts'))

config = Configuration()

config.section_('General')
config.General.requestName = ''
# if (args.workArea != ''):
#   config.General.workArea = args.workArea

config.section_('JobType')
config.JobType.pluginName = 'PrivateMC'
config.JobType.psetName = str(Path('do_nothing_cfg.py'))
scripts = resources.files('CombineHarvester.CombineTools.scripts')
config.JobType.scriptExe = ''
config.JobType.inputFiles = [
    str(scripts / 'FrameworkJobReport.xml'),
    str(scripts / 'copyRemoteWorkspace.sh'),
    os.path.join(ch_base or '', 'bin', scram_arch or '', 'combine')
]
config.JobType.outputFiles = ['combine_output.tar']
# config.JobType.maxMemoryMB = args.maxMemory

config.section_('Data')
config.Data.outputPrimaryDataset = 'Combine'
config.Data.splitting = 'EventBased'
config.Data.unitsPerJob = 1
config.Data.totalUnits = 1
config.Data.publication = False
config.Data.outputDatasetTag = ''

config.section_('User')

config.section_('Site')
config.Site.blacklist = ['T3_IT_Bologna','T3_US_UMiss']
config.Site.storageSite = 'T2_CH_CERN'
