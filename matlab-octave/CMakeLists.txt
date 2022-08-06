########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.7)
project(SoapySDRMex CXX)
enable_testing()

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

########################################################################
# Matlab (TODO)
########################################################################
message(STATUS "")
message(STATUS "#############################################")
message(STATUS "## Begin configuration for Matlab support...")
message(STATUS "#############################################")
message(STATUS "Enabling optional Matlab bindings if possible...")
find_package(Matlab)

include(FeatureSummary)
include(CMakeDependentOption)
cmake_dependent_option(ENABLE_MATLAB "Enable Matlab bindings" ON "ENABLE_LIBRARY;MATLAB_FOUND" OFF)
add_feature_info(Matlab ENABLE_MATLAB "Matlab bindings")
if(ENABLE_MATLAB)
endif()

########################################################################
# Octave
########################################################################
message(STATUS "")
message(STATUS "#############################################")
message(STATUS "## Begin configuration for Octave support...")
message(STATUS "#############################################")
message(STATUS "Enabling optional Octave bindings if possible...")
find_package(Octave)

include(FeatureSummary)
include(CMakeDependentOption)
cmake_dependent_option(ENABLE_OCTAVE "Enable Octave bindings" ON "ENABLE_LIBRARY;OCTAVE_FOUND" OFF)
add_feature_info(Octave ENABLE_OCTAVE "Octave bindings v${OCTAVE_VERSION_STRING}")
if(ENABLE_OCTAVE)
    # TODO: still the case without SWIG?
    if(APPLE AND CMAKE_COMPILER_IS_GNUCXX)
        message(WARNING "Octave bindings built with GCC on OS X are not officially supported and may not build.")
    endif()

    set(include_dirs
        ${CMAKE_CURRENT_SOURCE_DIR}/native/include
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}/include)

    # TODO: get this from target
    set(library_dirs
        ${PROJECT_BINARY_DIR}/lib)

    set(libraries
        SoapySDR)

    string(REPLACE ";" ";-I" include_flags "-I${include_dirs}")
    string(REPLACE ";" ";-L" libdir_flags "-L${library_dirs}")
    string(REPLACE ";" ";-l" library_flags "-l${libraries}")

    # TODO: not portable
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR_MEX.oct
        COMMENT "Building Octave bindings"
        COMMAND ${OCTAVE_MKOCTFILE_EXECUTABLE} ${include_flags} ${libdir_flags} ${library_flags} ${CMAKE_CURRENT_SOURCE_DIR}/native/SoapySDR_MEX.cpp
        DEPENDS SoapySDR
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/native/SoapySDR_MEX.cpp)

    add_custom_target(Octave_SoapySDR ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR_MEX.oct
        SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/native/SoapySDR_MEX.cpp)
endif()
