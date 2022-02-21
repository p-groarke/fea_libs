import os
from conans import ConanFile, CMake, tools
from conans.errors import ConanInvalidConfiguration

class Mnmesh2Conan(ConanFile):
    name = "fea_libs"
    version = "1.0.0"
    description = "A collection of useful libraries and utilities."
    topics = ("maps", "containers", "algorithms")
    url = "https://github.com/p-groarke/fea_libs"
    homepage = "https://github.com/p-groarke/fea_libs"
    license = "BSD-3"
    generators = "cmake_find_package_multi"
    settings = "os", "compiler", "build_type", "arch"
    requires = [
        ("gtest/1.11.0")
        , ("onetbb/2021.3.0")
        , ("date/3.0.0")
    ]
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

    def build(self):
        # for patch in self.conan_data.get("patches", {}).get(self.version, []):
        #     tools.patch(**patch)

        cmake = self._get_cmake()
        cmake.configure()
        cmake.build()

    def package(self):
        # Allows us to run the local workflow :
        # conan export-pkg . 3dsmax/local --build-folder=build -f
        # https://github.com/conan-io/conan/issues/2350

        cmake = self._get_cmake()
        cmake.configure()
        cmake.install()

        # Copy pdbs
        self.copy("*.pdb", src="bin", dst="bin")

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)

