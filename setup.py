import os
import pathlib
import subprocess
from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext

class CMakeExtension(Extension):
    def __init__(self, name):
        super().__init__(name, sources=[])

class CMakeBuild(build_ext):
    def build_extension(self, ext):
        build_temp = pathlib.Path(self.build_temp)
        build_temp.mkdir(parents=True, exist_ok=True)
        install_dir = build_temp / "install"
        cfg_cmd = ["cmake", str(pathlib.Path('.').resolve()), f"-DCMAKE_INSTALL_PREFIX={install_dir}"]
        subprocess.check_call(cfg_cmd, cwd=build_temp)
        build_cmd = ["cmake", "--build", str(build_temp), "--target", "install"]
        subprocess.check_call(build_cmd)
        target_dir = pathlib.Path(self.get_finalized_command('build_py').build_lib) / "CombineHarvester" / "CombineTools"
        target_dir.mkdir(parents=True, exist_ok=True)
        for lib in (install_dir / "CombineHarvester" / "CombineTools").glob("libCombineHarvester*.so"):
            self.copy_file(str(lib), str(target_dir / lib.name))

setup(
    ext_modules=[CMakeExtension("combineharvester_build")],
    cmdclass={"build_ext": CMakeBuild},
)
