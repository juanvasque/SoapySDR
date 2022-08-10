set(m_files
    ${CMAKE_CURRENT_SOURCE_DIR}/common/m/SoapySDR_ArgInfoType.m
    ${CMAKE_CURRENT_SOURCE_DIR}/common/m/SoapySDR_clearLogHandler.m
    ${CMAKE_CURRENT_SOURCE_DIR}/common/m/SoapySDR_Device.m
    ${CMAKE_CURRENT_SOURCE_DIR}/common/m/SoapySDR_Direction.m
    ${CMAKE_CURRENT_SOURCE_DIR}/common/m/SoapySDR_ErrorCode.m
    ${CMAKE_CURRENT_SOURCE_DIR}/common/m/SoapySDR_logf.m
    ${CMAKE_CURRENT_SOURCE_DIR}/common/m/SoapySDR_LogLevel.m
    ${CMAKE_CURRENT_SOURCE_DIR}/common/m/SoapySDR_log.m
    ${CMAKE_CURRENT_SOURCE_DIR}/common/m/SoapySDR_registerLogHandler.m
    ${CMAKE_CURRENT_SOURCE_DIR}/common/m/SoapySDR_RxStream.m
    ${CMAKE_CURRENT_SOURCE_DIR}/common/m/SoapySDR_setLogLevel.m
    ${CMAKE_CURRENT_SOURCE_DIR}/common/m/SoapySDR_StreamFlag.m
    ${CMAKE_CURRENT_SOURCE_DIR}/common/m/SoapySDR_StreamFormat.m
    ${CMAKE_CURRENT_SOURCE_DIR}/common/m/SoapySDR_Stream.m
    ${CMAKE_CURRENT_SOURCE_DIR}/common/m/SoapySDR_ticksToTimeNs.m
    ${CMAKE_CURRENT_SOURCE_DIR}/common/m/SoapySDR_timeNsToTicks.m
    ${CMAKE_CURRENT_SOURCE_DIR}/common/m/SoapySDR_TxStream.m)

set(test_names
    TestTimeConversion)

add_subdirectory(matlab)
add_subdirectory(octave)
