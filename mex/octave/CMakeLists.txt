list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR})

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
if(NOT ENABLE_OCTAVE)
    return()
endif()

set(include_dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/native/include
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
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR_MEX.mex
    COMMENT "Building SoapySDR_MEX.mex for Octave"
    COMMAND ${OCTAVE_MKOCTFILE_EXECUTABLE} --mex ${include_flags} ${libdir_flags} ${library_flags} ${CMAKE_CURRENT_SOURCE_DIR}/../common/native/SoapySDR_MEX.cpp
    DEPENDS SoapySDR
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../common/native/SoapySDR_MEX.cpp)

add_custom_target(SoapySDR_MEX_Octave ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR_MEX.mex
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../common/native/SoapySDR_MEX.cpp)

########################################################################
# Testing
########################################################################

# .m files need to be in the same directory as the corresponding MEX
# file, so copy them and execute the tests here.
foreach(src ${m_files})
    # TODO: copy to target output directory
    file(COPY ${src} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
