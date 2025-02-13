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
            f"-DBUILD_TESTING=OFF",
            f"-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
        ]

        build_temp = Path(self.build_temp) / ext.name
        if not build_temp.exists():
            build_temp.mkdir(parents=True)

        conan_install = ["conan", "install", ext.sourcedir, "--build=missing", "--profile", "profiles//msvc-ninja-x64"]
        print(" ".join(conan_install))
        cmake_configure = [
            f'D:/programs/cmake/bin/cmake.EXE', 
            '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON', 
            '-Dpasio_DEVELOPER_MODE=ON',
            '-DCMAKE_POLICY_DEFAULT_CMP0091=NEW', 
            '-Dpasio_ENABLE_CPPCHECK=ON',
            '-Dpasio_ENABLE_CLANGTIDY=ON', 
            '-DCMAKE_CXX_FLAGS=/sdl /guard:cf /utf-8 /diagnostics:caret /w14165 /w44242 /w44254 /w44263 /w34265 /w34287 /w44296 /w44365 /w44388 /w44464 /w14545 /w14546 /w14547 /w14549 /w14555 /w34619 /w34640 /w24826 /w14905 /w14906 /w14928 /w45038 /W4 /permissive- /volatile:iso /Zc:inline /Zc:preprocessor /Zc:enumTypes /Zc:lambda /Zc:__cplusplus /Zc:externConstexpr /Zc:throwingNew /EHsc', 
            '-DCMAKE_EXE_LINKER_FLAGS=/machine:x64 /guard:cf',
            '-DCMAKE_CXX_EXTENSIONS=OFF', 
            '-DCMAKE_CXX_STANDARD=20', '-DCMAKE_CXX_STANDARD_REQUIRED=ON',
            '-DCMAKE_TOOLCHAIN_FILE=D:/coding/cpp/pasio/conan/Windows/conan_toolchain.cmake',
            '-SD:/coding/cpp/pasio',
            '-BD:/coding/cpp/pasio/build', 
            '-G',
            "Visual Studio 17 2022",
            '-A', 
            "x64", 
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}{os.sep}"
            ]
        # cmake_configure = ["cmake", "--fresh", "-A", "x64", "-S", ext.sourcedir, "-B", f"{ext.sourcedir}/build", "--toolchain", f"{ext.sourcedir}//conan//Windows//conan_toolchain.cmake", *cmake_args]
        print(" ".join(conan_install))
        cmake_build = ["cmake", "--build", f"{ext.sourcedir}/build", "--parallel", "8"]

        subprocess.run(conan_install, check=True)
        subprocess.run(cmake_configure, check=True)
        subprocess.run(cmake_build, check=True)

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
