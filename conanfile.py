import os
import re
import toml

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout


class ProjectRecipe(ConanFile):
    package_type = "library"

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # Sources are located in the same place as this recipe, copy them to the recipe
    # way of simplify is to use "*" - export all files
    exports_sources = (
        "CMakeLists.txt",
        "dependencies.cmake",
        "options.cmake",
        "pasio/include/*",
        "pasio/src/*",
        "pasio/cmake/*",
        "pasio/CMakeLists.txt",
        "cmake/*",
    )

    @property
    def _project_components(self):
        def fmt():
            return ["fmt::fmt"]

        project_components = [
            {
                "target": "core",
                "lib": "core",
                "requires": (["utility"] + fmt()),
            },
        ]

        return project_components
    
    def _get_pyproject_data(self):
        try:
            pyproject_path = os.path.join(self.recipe_folder, "pyproject.toml")
            with open(pyproject_path, "r", encoding="utf-8") as f:
                return toml.load(f)
        except FileNotFoundError as error:
            print(f"Failed to load pyproject.toml content with error: {error}")

    def set_name(self):
        pyproject_content = self._get_pyproject_data()
        self.name = pyproject_content["project"]["name"]

    def set_version(self):
        pyproject_content = self._get_pyproject_data()
        self.name = pyproject_content["project"]["version"]
            
    def requirements(self):
        self.requires("asio/1.32.0")
        self.requires("pybind11/2.13.6")
        self.test_requires("catch2/3.8.0")

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)
        self.folders.generators = "conan/Windows"

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        debug = (
            "d"
            if self.settings.build_type == "Debug" and self.settings.os == "Windows"
            else ""
        )

        def get_lib_name(module):
            return f"{module}{debug}" if module else None

        self.cpp_info.set_property("cmake_find_mode", "both")
        self.cpp_info.set_property("cmake_file_name", f"{self.name}")

        for component in self._project_components:
            target = component["target"]
            lib = get_lib_name(component.get("lib", None))

            self.cpp_info.components[target].set_property(
                "cmake_target_name", f"{self.name}::{target}"
            )
            self.cpp_info.components[target].libs = (
                [f"{self.name}-{lib}"] if lib else []
            )

            requirements = component.get("requires", [])
            self.cpp_info.components[target].requires = requirements
