import os
import re
import subprocess
from pathlib import Path
from dataclasses import dataclass
import os
import subprocess
import sys
from setuptools import setup
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from setuptools.command.sdist import sdist
from pathlib import Path
from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext
from setuptools import setup, find_packages

@dataclass
class Preset:
    cmake_configure_preset: str
    cmake_build_preset: str
    conan_profile: str

PLAT_TO_CONAN = {
    "win32": Preset("ci-windows32-ninja", "ci-windows32-ninja-release", "profiles//msvc-ninja-x32"),
    "win-amd64": Preset("ci-windows-ninja", "ci-windows-ninja-release", "profiles//msvc-ninja-x64"),
    "linux-x86_64": Preset("ci-ubuntu-release", "ci-ubuntu-release", "profiles/gcc-13-x64"),
}


# A CMakeExtension needs a sourcedir instead of a file list.
# The name must be the _single_ output extension from the CMake build.
# If you need multiple extensions, see scikit-build.
class CMakeExtension(Extension):
    def __init__(self, name: str, sourcedir: str = "") -> None:
        super().__init__(name, sources=[])
        self.sourcedir = os.fspath(Path(sourcedir).resolve())


class CMakeBuild(build_ext):
    def build_extension(self, ext: CMakeExtension) -> None:
        try:
            subprocess.check_output(["cmake", "--version"])
        except OSError:
            raise RuntimeError("CMake is not available.") from None
        
        try:
            subprocess.check_output(["ninja", "--version"])
        except OSError:
            raise RuntimeError("Ninja is not available.") from None
        
        try:
            subprocess.check_output(["conan", "--version"])
        except OSError:
            raise RuntimeError("Conan is not available.") from None
        
        ext_fullpath = Path.cwd() / self.get_ext_fullpath(ext.name)
        extdir = ext_fullpath.parent.resolve()

        platfrom: str = self.plat_name
        print(f"platform is {platfrom}")

        build_pair = PLAT_TO_CONAN[platfrom]
        cmake_preset = build_pair.cmake_configure_preset
        cmake_build = build_pair.cmake_build_preset
        conan_profile = build_pair.conan_profile

        conan_install = ["conan", "install", ".", "-pr", conan_profile, "--build=missing"]
        print(" ".join(conan_install))
        cmake_configure = ["cmake", "--preset", cmake_preset, "-DBUILD_TESTING=OFF -Dpasio_BUILD_DOCS=OFF"]
        print(" ".join(cmake_configure))
        cmake_build = ["cmake", "--build", "--preset", f"{cmake_build}"]
        print(" ".join(cmake_build))
        cmake_install = ["cmake", "--install",  f"build/{cmake_preset}", "--component pasio_Wheel", "--prefix", f"{extdir}"]
        print(" ".join(cmake_install))

        subprocess.run(conan_install)
        subprocess.run(cmake_configure)
        subprocess.run(cmake_build)
        subprocess.run(cmake_install)

# The information here can also be placed in setup.cfg - better separation of
# logic and declaration, and simpler if you include description/version in a file.
setup(
    name="pasio",
    version="0.1.0",
    author="https://github.com/anmayithap",
    author_email="https://github.com/anmayithap",
    description="Bindings for asio::serial_port",
    long_description="",
    packages=find_packages(include=["pasio"]),  # Укажите только нужные пакеты
    ext_modules=[CMakeExtension("pasio")],
    cmdclass={"build_ext": CMakeBuild},
    zip_safe=False,
    extras_require={},
    python_requires=">=3.10",
)
