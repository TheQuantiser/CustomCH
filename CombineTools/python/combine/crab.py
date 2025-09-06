#!/usr/bin/env python3
import os
import shutil
import CombineHarvester.CombineTools.ch as ch
from WMCore.Configuration import Configuration


config = Configuration()

config.section_('General')
config.General.requestName = ''
# if (args.workArea != ''):
#   config.General.workArea = args.workArea

config.section_('JobType')
config.JobType.pluginName = 'PrivateMC'
script_dir = os.path.join(ch.paths.base(), 'CombineTools/scripts')
config.JobType.psetName = os.path.join(script_dir, 'do_nothing_cfg.py')
config.JobType.scriptExe = ''
combine_exec = shutil.which('combine') or 'combine'
config.JobType.inputFiles = [
    os.path.join(script_dir, 'FrameworkJobReport.xml'),
    os.path.join(script_dir, 'copyRemoteWorkspace.sh'),
    combine_exec
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
