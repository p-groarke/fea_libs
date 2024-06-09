import os, platform
from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import collect_libs, copy

class FeaLibsConan(ConanFile):
    name = "fea_libs"
    description = "A collection of useful libraries and utilities."
    topics = ("maps", "containers", "algorithms")
    url = "https://github.com/p-groarke/fea_libs"
    homepage = "https://github.com/p-groarke/fea_libs"
    license = "BSD-3"
    generators = "CMakeDeps", "CMakeToolchain"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "fPIC": [True, False],
        "with_tbb": [True, False],
        "with_onetbb": [True, False],
    }
    default_options = {
        "fPIC": True,
        "with_tbb": True,
        "with_onetbb": False,
        "gtest/*:build_gmock" : False,
        "date/*:use_system_tz_db" : True,
        "tbb/*:tbbmalloc" : False,
        "tbb/*:tbbproxy" : False,
        "tbb/*:shared" : True,
        "onetbb/*:tbbmalloc" : False,
        "onetbb/*:tbbproxy" : False,
        # "onetbb/*:shared" : True,
    }
    exports_sources = ["*", "!build/*", "!build_reports/*", "!Output/*", "!bin/*"]

    def requirements(self):
        self.requires("gtest/1.11.0#7475482232fcd017fa110b0b8b0f936e", "private")
        self.requires("date/3.0.0#8fcb40f84e304971b86cae3c21d2ce99")
        # self.requires("pkgconf/2.2.0", override=True)

        if self.options.with_onetbb:
            # Prioritize onetbb.
            self.requires("onetbb/2021.12.0")
        elif self.options.with_tbb:
            self.requires("onetbb/2020.3#47de209cf102270d266f4b20e4524d0b")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    # def layout(self):
    #     cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
        tc = CMakeDeps(self)
        tc.generate()

        # imports
        for dep in self.dependencies.values():
            copy(self, "*.dl*", src=dep.cpp_info.bindirs[0], dst=os.path.join(self.build_folder, "bin"))
            copy(self, "*.dylib*", src=dep.cpp_info.bindirs[0], dst=os.path.join(self.build_folder, "bin")) # libdirs?
            copy(self, "*.so*", src=dep.cpp_info.bindirs[0], dst=os.path.join(self.build_folder, "bin")) # libdirs?
            copy(self, "*.pdb", src=dep.cpp_info.bindirs[0], dst=os.path.join(self.build_folder, "bin"))
            # copy(self, "*.pdb", src=dep.cpp_info.bindirs[0], dst=os.path.join(self.build_folder, "lib"))

    def build(self):
        cmake = CMake(self)
        mdefinitions = {
            "FEA_TESTS": False,
            "FEA_BENCHMARKS": False,
            "FEA_PULL_CONAN": False,
            "FEA_WITH_TBB": self.options.with_tbb,
            "FEA_WITH_ONETBB": self.options.with_onetbb,
        }
        cmake.configure(variables=mdefinitions)
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

        # Copy pdbs
        copy(self, "*.pdb", src=os.path.join(self.build_folder, "bin"), dst=os.path.join(self.package_folder, "bin"))

    def package_info(self):
        self.cpp_info.libs = collect_libs(self)

