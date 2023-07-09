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

    def requirements(self):
        self.requires("gtest/1.11.0#7475482232fcd017fa110b0b8b0f936e", "private")
        self.requires("date/3.0.0#8fcb40f84e304971b86cae3c21d2ce99")

        if self.settings.os == "Macos":
            # Fix stdlib.h include.
            self.requires("onetbb/2021.3.0#98a11c269d52fa4756d43d6faf640792")
        else:
            self.requires("onetbb/2020.3#0fa586916737df2aa07f4f34ede163cc")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def imports(self):
       self.copy("*.dl*", src="bin", dst="bin")
       self.copy("*.pdb", src="bin", dst="bin")
       self.copy("*.pdb", src="bin", dst="lib")

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

