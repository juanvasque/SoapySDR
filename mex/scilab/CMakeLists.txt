list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "")
message(STATUS "#############################################")
message(STATUS "## Begin configuration for Scilab support...")
message(STATUS "#############################################")
message(STATUS "Enabling optional Scilab bindings if possible...")
find_package(Scilab)

include(FeatureSummary)
include(CMakeDependentOption)
cmake_dependent_option(ENABLE_SCILAB "Enable Scilab bindings" ON "UNIX;CMAKE_COMPILER_IS_GNUCXX;ENABLE_LIBRARY;SCILAB_FOUND" OFF)
add_feature_info(Scilab ENABLE_SCILAB "Scilab bindings")
if(NOT ENABLE_SCILAB)
    return()
endif()

########################################################################
# Build MEX
########################################################################
set(MEX_FILEPATH ${CMAKE_CURRENT_SOURCE_DIR}/../native/SoapySDR_MEX.cpp)

configure_file(
    builder.in.sce
    ${CMAKE_CURRENT_BINARY_DIR}/builder.sce
@ONLY)

set(TARGET_DEPENDS
    ${MEX_FILEPATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/builder.in.sce
    ${CMAKE_CURRENT_BINARY_DIR}/builder.sce)

# TODO: not portable
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libSoapySDR_MEX.so
    COMMENT "Building SoapySDR_MEX for Scilab"
    COMMAND ${SCILAB_CLI} -f ${CMAKE_CURRENT_BINARY_DIR}/builder.sce -quit
    DEPENDS SoapySDR
    DEPENDS ${TARGET_DEPENDS})

add_custom_target(SoapySDR_MEX_Scilab ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libSoapySDR_MEX.so
    SOURCES ${TARGET_DEPENDS})
