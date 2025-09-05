from __future__ import absolute_import

try:
    import FWCore.ParameterSet.Config as cms
except ImportError as exc:  # pragma: no cover - runtime environment
    raise ImportError(
        "FWCore is required to run this configuration. "
        "Ensure that a CMSSW environment is active."
    ) from exc
process = cms.Process("MAIN")

process.source = cms.Source("EmptySource")

