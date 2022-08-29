find_program(SCILAB_CLI
    NAMES scilab-cli
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Scilab REQUIRED_VARS SCILAB_CLI)

mark_as_advanced(SCILAB_CLI)
