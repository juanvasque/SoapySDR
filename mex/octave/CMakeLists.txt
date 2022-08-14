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

if(CMAKE_COMPILER_IS_CXX OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    set(other_flags "-std=c++11")
endif()

# TODO: not portable
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR_MEX.mex
    COMMENT "Building SoapySDR_MEX.mex for Octave"
    COMMAND ${OCTAVE_MKOCTFILE_EXECUTABLE} --mex ${include_flags} ${libdir_flags} ${library_flags} ${other_flags} ${CMAKE_CURRENT_SOURCE_DIR}/../common/native/SoapySDR_MEX.cpp
    DEPENDS SoapySDR
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../common/native/SoapySDR_MEX.cpp)

add_custom_target(SoapySDR_MEX_Octave ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR_MEX.mex
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../common/native/SoapySDR_MEX.cpp)

install(
    FILES ${m_files}
    DESTINATION ${OCTAVE_M_SITE_DIR}/SoapySDR)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR_MEX.mex
    DESTINATION ${OCTAVE_OCT_SITE_DIR})

########################################################################
# Testing
########################################################################

execute_process(
    COMMAND ${OCTAVE_EXECUTABLE} --eval "moxunit_set_path()"
    RESULT_VARIABLE OCTAVE_FOUND_MOXUNIT
    OUTPUT_QUIET
    ERROR_QUIET
    )
if(OCTAVE_FOUND_MOXUNIT EQUAL 0)
    message(STATUS "Checking for MOxUnit with Octave - found")
    set(OCTAVE_FOUND_MOXUNIT TRUE)
else()
    message(STATUS "Checking for MOxUnit with Octave - failed")
    set(OCTAVE_FOUND_MOXUNIT FALSE)
endif()

include(FeatureSummary)
include(CMakeDependentOption)
cmake_dependent_option(ENABLE_OCTAVE_TESTING "Enable Octave unit tests" ON "ENABLE_OCTAVE;ENABLE_TESTS;OCTAVE_FOUND_MOXUNIT" OFF)
add_feature_info("Octave tests" ENABLE_OCTAVE_TESTING "Octave unit tests")
if(NOT ENABLE_OCTAVE_TESTING)
    return()
endif()

# Copy the .m files to the test directory so we don't have to deal
# with path weirdness.
foreach(src ${m_files})
    # TODO: copy to target output directory
    file(COPY ${src} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

foreach(test ${test_names})
    set(test_path ${CMAKE_CURRENT_SOURCE_DIR}/../common/tests/${test}.m)

    add_test(
        NAME Octave_${test}
        COMMAND ${OCTAVE_EXECUTABLE} --eval "moxunit_set_path(); moxunit_runtests(filename='${test_path}')"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    set_tests_properties(Octave_${test} PROPERTIES
        FAIL_REGULAR_EXPRESSION "FAILED")
endforeach()
