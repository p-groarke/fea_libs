import os, platform
from conan import ConanFile, conan_version
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import collect_libs, copy
from conan.tools.scm import Version
from conan.tools.env import VirtualBuildEnv

class FeaLibsConan(ConanFile):
    name = "fea_libs"
    description = "A collection of useful libraries and utilities."
    topics = ("maps", "containers", "algorithms")
    url = "https://github.com/p-groarke/fea_libs"
    homepage = "https://github.com/p-groarke/fea_libs"
    license = "BSD-3"
    # generators = "CMakeDeps", "CMakeToolchain" # Which is better?
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "fPIC": [True, False],
        "with_tbb": [True, False],
        "with_onetbb": [True, False],
        "with_date": [True, False],
    }
    default_options = {
        "fPIC": True,
        "with_tbb": True,
        "with_onetbb": False,
        "with_date": True,
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
        if Version(conan_version).major < "2":
            self.requires("gtest/1.15.0", "private")
        else:
            self.requires("gtest/1.15.0", test=True)
        
        if self.options.with_date:
            self.requires("date/3.0.0")

        if self.options.with_onetbb:
            # Prioritize onetbb.
            self.requires("onetbb/2021.12.0")
        elif self.options.with_tbb:
            self.requires("onetbb/2020.3.3")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    # def layout(self):
    #     cmake_layout(self)

    def generate(self):
        tc = VirtualBuildEnv(self)
        tc.generate()

        tc = CMakeToolchain(self)
        tc.cache_variables["FEA_PULL_CONAN"] = False
        tc.cache_variables["FEA_TESTS"] = False
        tc.cache_variables["FEA_BENCHMARKS"] = False
        tc.cache_variables["FEA_WITH_TBB"] = self.options.with_tbb
        tc.cache_variables["FEA_WITH_ONETBB"] = self.options.with_onetbb
        tc.cache_variables["FEA_WITH_DATE"] = self.options.with_date
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
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

        # Copy pdbs
        copy(self, "*.pdb", src=os.path.join(self.build_folder, "bin"), dst=os.path.join(self.package_folder, "bin"))

    def package_info(self):
        self.cpp_info.libs = collect_libs(self)

