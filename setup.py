import os
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
    user_options = build_ext.user_options + [
        ("build-threads=", None, "number of threads for building"),
    ]

    def initialize_options(self) -> None:  # noqa: D401 - see base class
        super().initialize_options()
        self.build_threads = None

    def finalize_options(self) -> None:  # noqa: D401 - see base class
        super().finalize_options()
        if self.build_threads is not None:
            try:
                self.build_threads = int(self.build_threads)
            except ValueError as err:
                raise ValueError("--build-threads must be an integer") from err

    def build_extension(self, ext: Extension) -> None:  # noqa: D401 - see base class
        source_dir = Path(__file__).resolve().parent

        suffix = ".dll" if sys.platform == "win32" else (
            ".dylib" if sys.platform == "darwin" else ".so"
        )
        lib_name = f"libCombineHarvesterCombineTools{suffix}"

        search_dirs = [Path(sys.prefix) / "lib", Path(sys.prefix) / "lib64"]
        search_dirs.extend(
            Path(p) for p in os.environ.get("LD_LIBRARY_PATH", "").split(os.pathsep) if p
        )
        search_dirs.append(source_dir / ".python" / "CombineHarvester" / "CombineTools")

        for directory in search_dirs:
            candidate = directory / lib_name
            if candidate.is_file():
                print(f"Reusing prebuilt CombineTools at {candidate}")

                pkg_dir = source_dir / ".python" / "CombineHarvester" / "CombineTools"
                pkg_dir.mkdir(parents=True, exist_ok=True)
                shutil.copy2(candidate, pkg_dir / lib_name)

                dest_dir = Path(self.build_lib) / "CombineHarvester" / "CombineTools"
                dest_dir.mkdir(parents=True, exist_ok=True)
                shutil.copy2(candidate, dest_dir / lib_name)
                return

        print("CombineTools library not found—building from source")

        build_temp = Path(self.build_temp)
        build_temp.mkdir(parents=True, exist_ok=True)
        subprocess.check_call(
            [
                "cmake",
                "-S",
                str(source_dir),
                "-B",
                str(build_temp),
                f"-DPython_EXECUTABLE={sys.executable}",
                "-DUSE_SYSTEM_COMBINEDLIMIT=ON",
                f"-DCMAKE_PREFIX_PATH={sys.prefix}",
            ]
        )
        build_cmd = ["cmake", "--build", str(build_temp), "--target", "CombineTools"]
        if self.build_threads is not None:
            build_cmd.extend(["--parallel", str(self.build_threads)])
        else:
            build_cmd.append("--parallel")
        subprocess.check_call(build_cmd)

        built_lib = build_temp / "CombineTools" / lib_name

        pkg_dir = source_dir / ".python" / "CombineHarvester" / "CombineTools"
        pkg_dir.mkdir(parents=True, exist_ok=True)
        shutil.copy2(built_lib, pkg_dir / lib_name)

        dest_dir = Path(self.build_lib) / "CombineHarvester" / "CombineTools"
        dest_dir.mkdir(parents=True, exist_ok=True)
        shutil.copy2(built_lib, dest_dir / lib_name)


setup(
    cmdclass={"build_ext": CMakeBuild},
    ext_modules=[CMakeExtension("combineharvester_dummy")],
)

