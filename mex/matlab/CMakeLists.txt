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
if(NOT ENABLE_MATLAB)
    return()
endif()
