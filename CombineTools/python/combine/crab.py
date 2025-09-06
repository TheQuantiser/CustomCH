from __future__ import absolute_import
import os
import os
import CombineHarvester.CombineTools.ch as ch
from WMCore.Configuration import Configuration


config = Configuration()

config.section_('General')
config.General.requestName = ''
# if (args.workArea != ''):
#   config.General.workArea = args.workArea

config.section_('JobType')
config.JobType.pluginName = 'PrivateMC'
ch_base = os.environ.get('CH_BASE', ch.paths.base())
config.JobType.psetName = ch_base + '/CombineTools/scripts/do_nothing_cfg.py'
config.JobType.scriptExe = ''
config.JobType.inputFiles = [
    ch_base + '/CombineTools/scripts/FrameworkJobReport.xml',
    ch_base + '/CombineTools/scripts/copyRemoteWorkspace.sh',
    ch_base + '/bin/' + os.environ['SCRAM_ARCH'] + '/combine'
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
