from __future__ import absolute_import
import os
from WMCore.Configuration import Configuration


cmssw_base = os.environ.get('CMSSW_BASE')
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
config.JobType.psetName = os.path.join(scripts_dir, 'do_nothing_cfg.py')
config.JobType.scriptExe = ''
input_files = [
    os.path.join(scripts_dir, 'FrameworkJobReport.xml'),
    os.path.join(scripts_dir, 'copyRemoteWorkspace.sh')
]
if combine_path:
    input_files.append(combine_path)
elif cmssw_base and scram_arch:
    input_files.append(os.path.join(cmssw_base, 'bin', scram_arch, 'combine'))
config.JobType.inputFiles = input_files
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
