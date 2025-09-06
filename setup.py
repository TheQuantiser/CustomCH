import subprocess
import sys
import shutil
from pathlib import Path

from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext


class CMakeExtension(Extension):
    def __init__(self, name: str) -> None:
        super().__init__(name, sources=[])


class CMakeBuild(build_ext):
    def build_extension(self, ext: Extension) -> None:  # noqa: D401 - see base class
        build_temp = Path(self.build_temp)
        build_temp.mkdir(parents=True, exist_ok=True)
        source_dir = Path(__file__).resolve().parent
        subprocess.check_call(["cmake", "-S", str(source_dir), "-B", str(build_temp)])
        subprocess.check_call(["cmake", "--build", str(build_temp), "--target", "CombineTools"])

        suffix = ".dll" if sys.platform == "win32" else (".dylib" if sys.platform == "darwin" else ".so")
        lib_name = f"libCombineHarvesterCombineTools{suffix}"
        built_lib = build_temp / "CombineTools" / lib_name

        # Copy into the package source for editable installs
        pkg_dir = source_dir / "CombineTools" / "python"
        pkg_dir.mkdir(parents=True, exist_ok=True)
        shutil.copy2(built_lib, pkg_dir / lib_name)

        # Ensure the library is also available in the build directory
        dest_dir = Path(self.build_lib) / "CombineHarvester" / "CombineTools"
        dest_dir.mkdir(parents=True, exist_ok=True)
        shutil.copy2(built_lib, dest_dir / lib_name)


setup(
    cmdclass={"build_ext": CMakeBuild},
    ext_modules=[CMakeExtension("combineharvester_dummy")],
)

