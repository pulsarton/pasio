import os
import re
import subprocess
import sys
from pathlib import Path

from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext
from setuptools import setup, find_packages

PLAT_TO_CONAN = {
    "win-amd64": "msvc-ninja-x64"
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
        # Must be in this form due to bug in .resolve() only fixed in Python 3.10+
        ext_fullpath = Path.cwd() / self.get_ext_fullpath(ext.name)
        extdir = ext_fullpath.parent.resolve()

        # Using this requires trailing slash for auto-detection & inclusion of
        # auxiliary "native" libs

        cfg = "Release"

        # Set Python_EXECUTABLE instead if you use PYBIND11_FINDPYTHON
        # EXAMPLE_VERSION_INFO shows you how to pass a value into the C++ code
        # from Python.
        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}{os.sep}",
            f"-DPython3_EXECUTABLE={sys.executable}",
            f"-DCMAKE_BUILD_TYPE={cfg}",  # not used on MSVC, but no harm
            f"-DBUILD_TESTING=OFF"
        ]

        build_temp = Path(self.build_temp) / ext.name
        if not build_temp.exists():
            build_temp.mkdir(parents=True)

        print(" ".join(["conan", "install", ext.sourcedir, "--build=missing", "--profile", "profiles\\msvc-ninja-x64"]))
        print(" ".join(["cmake", "-G Ninja", "-S", ext.sourcedir, "-B", "build", "--toolchain", "conan\\Windows\\conan_toolchain.cmake",  *cmake_args, "--fresh"]))
        print(" ".join(["cmake", "--build", "build", "--parallel", "8"]))

        subprocess.run(
            ["conan", "install", ext.sourcedir, "--build=missing", "--profile", "profiles\\msvc-ninja-x64"], check=True
        )
        subprocess.run(
            ["cmake", "-G Ninja", "-S", ext.sourcedir, "-B", "build", "--toolchain", "conan\\Windows\\conan_toolchain.cmake",  *cmake_args, "--fresh"], check=True, shell=True
        )
        subprocess.run(
            ["cmake", "--build", "build", "--parallel", "8"], check=True, shell=True
        )


# The information here can also be placed in setup.cfg - better separation of
# logic and declaration, and simpler if you include description/version in a file.
setup(
    name="pasio",
    version="0.1.0",
    author="https://github.com/anmayithap",
    author_email="https://github.com/anmayithap",
    description="Bindings for asio::serial_port",
    long_description="",
    packages=find_packages(include=["core"]),  # Укажите только нужные пакеты
    ext_modules=[CMakeExtension("pasio")],
    cmdclass={"build_ext": CMakeBuild},
    zip_safe=False,
    extras_require={},
    python_requires=">=3.10",
)
