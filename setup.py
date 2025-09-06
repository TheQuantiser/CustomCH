import subprocess
import sys
import shutil
from pathlib import Path

from setuptools import setup
from setuptools.command.build_ext import build_ext


class CMakeBuild(build_ext):
    def run(self):
        super().run()
        build_temp = Path(self.build_temp)
        build_temp.mkdir(parents=True, exist_ok=True)
        source_dir = Path(__file__).resolve().parent
        subprocess.check_call(["cmake", "-S", str(source_dir), "-B", str(build_temp)])
        subprocess.check_call(["cmake", "--build", str(build_temp), "--target", "CombineTools", "CombinePdfs"])
        suffix = ".dll" if sys.platform == "win32" else (".dylib" if sys.platform == "darwin" else ".so")
        dest_tools = Path(self.build_lib) / "CombineHarvester" / "CombineTools"
        dest_pdfs = Path(self.build_lib) / "CombineHarvester" / "CombinePdfs"
        dest_tools.mkdir(parents=True, exist_ok=True)
        dest_pdfs.mkdir(parents=True, exist_ok=True)
        shutil.copy2(build_temp / "CombineTools" / f"libCombineTools{suffix}",
                     dest_tools / f"libCombineHarvesterCombineTools{suffix}")
        shutil.copy2(build_temp / "CombinePdfs" / f"libCombinePdfs{suffix}",
                     dest_pdfs / f"libCombineHarvesterCombinePdfs{suffix}")


setup(cmdclass={"build_ext": CMakeBuild})

