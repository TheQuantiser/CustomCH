from __future__ import absolute_import
import os
from pathlib import Path
from importlib.resources import files

# Prevent cppyy's check for the PCH
os.environ['CLING_STANDARD_PCH'] = 'none'
import cppyy


def _load_library() -> None:
    libname = "libCombineHarvesterCombinePdfs"
    search_dir = os.environ.get("CH_LIBRARY_PATH")
    if search_dir:
        base = Path(search_dir)
    else:
        base = Path(files(__package__))
    for ext in (".so", ".dylib"):
        candidate = base / f"{libname}{ext}"
        if candidate.exists():
            try:
                cppyy.load_reflection_info(str(candidate.resolve()))
            except OSError as err:
                raise OSError(
                    f"Failed to load {candidate}. Rebuild the project or set CH_LIBRARY_PATH"
                ) from err
            return
    raise OSError(
        f"Could not find {libname}. Rebuild the project or set CH_LIBRARY_PATH to its location."
    )


_load_library()

def BuildRooMorphing(ws, cb, bin, process, mass_var, norm_postfix='norm', allow_morph=True, verbose=False, force_template_limit=False, file=None):
    return cppyy.gbl.ch.BuildRooMorphing(ws, cb, bin, process, mass_var, norm_postfix, allow_morph, verbose, force_template_limit, file);

def BuildCMSHistFuncFactory(ws, cb, mass_vars, process):
  f = cppyy.gbl.ch.CMSHistFuncFactory()
  morphVars = cppyy.gbl.map['std::string','RooAbsReal*']()
  morphVars[process] = mass_vars
  f.SetHorizontalMorphingVariable(morphVars)
  f.Run(cb, ws)

