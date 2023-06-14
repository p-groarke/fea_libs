import os, platform
from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import collect_libs

class FeaLibsConan(ConanFile):
    name = "fea_libs"
    description = "A collection of useful libraries and utilities."
    topics = ("maps", "containers", "algorithms")
    url = "https://github.com/p-groarke/fea_libs"
    homepage = "https://github.com/p-groarke/fea_libs"
    license = "BSD-3"
    generators = "CMakeDeps"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "fPIC": [True, False]
    }
    default_options = {
        "fPIC": True,
        "gtest:build_gmock" : False,
        "date:use_system_tz_db" : True,
        "tbb:tbbmalloc" : True
    }
    exports_sources = ["*", "!build/*", "!build_reports/*", "!Output/*", "!bin/*"]

    requires = [
        ("gtest/1.11.0#7475482232fcd017fa110b0b8b0f936e")
        , ("tbb/2020.3#1368e21b74ba4c45faa2a73ff3cffd8a")
        , ("date/3.0.0#8fcb40f84e304971b86cae3c21d2ce99")
    ]

    _cmake = None

    # @property
    # def _source_subfolder(self):
    #     return "source_subfolder"

    # @property
    # def _build_subfolder(self):
    #     return "build_subfolder"


    def _get_cmake(self):
        if self._cmake:
            return self._cmake

        self._cmake = CMake(self)
        self._cmake.definitions["FEA_TESTS"] = False
        self._cmake.definitions["FEA_BENCHMARKS"] = False
        self._cmake.definitions["FEA_PULL_CONAN"] = False
        return self._cmake

    def imports(self):
       self.copy("*.dl*", src="bin", dst="bin")
       self.copy("*.pdb", src="bin", dst="bin")
       self.copy("*.pdb", src="bin", dst="lib")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    # def configure(self):

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
        tc = CMakeDeps(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        mdefinitions = {
            "FEA_TESTS": False,
            "FEA_BENCHMARKS": False,
            "FEA_PULL_CONAN": False,
        }
        cmake.configure(variables=mdefinitions)
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

        # Copy pdbs
        self.copy("*.pdb", src="bin", dst="bin")

    def package_info(self):
        self.cpp_info.libs = collect_libs(self)

