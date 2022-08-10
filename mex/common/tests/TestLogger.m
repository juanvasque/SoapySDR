# Copyright (c) 2022 Nicholas Corgan
# SPDX-License-Identifier: BSL-1.0

function test_suite=TestLogger
    try % assignment of 'localfunctions' is necessary in Matlab >= 2016
        test_functions=localfunctions();
    catch % no problem; early Matlab versions can use initTestSuite fine
    end
    initTestSuite;

function fcn = loggerFcn(level, message)
    printf("Global function: %d: %s\n", level, message)

function testLog
    SoapySDR;

    SoapySDR.setLogLevel(SoapySDR.LogLevel.TRACE)
    # TODO: test getter when implemented

    SoapySDR.log(SoapySDR.LogLevel.FATAL, "FATAL")
    SoapySDR.log(SoapySDR.LogLevel.CRITICAL, "CRITICAL")
    SoapySDR.log(SoapySDR.LogLevel.ERROR, "ERROR")
    SoapySDR.log(SoapySDR.LogLevel.WARNING, "WARNING")
    SoapySDR.log(SoapySDR.LogLevel.NOTICE, "NOTICE")
    SoapySDR.log(SoapySDR.LogLevel.INFO, "INFO")
    SoapySDR.log(SoapySDR.LogLevel.DEBUG, "DEBUG")
    SoapySDR.log(SoapySDR.LogLevel.TRACE, "TRACE")
    SoapySDR.log(SoapySDR.LogLevel.SSI, "SSI")
    puts("\n") # For the sake of formatting

    SoapySDR.setLogLevel(SoapySDR.LogLevel.NOTICE)
    SoapySDR.logf(SoapySDR.LogLevel.NOTICE, "Notice: %s %d %f", "str", 1351, 4.18)

    # TODO: make sure to document these can't be anonymous functions
    SoapySDR.registerLogger(@loggerFcn)
    SoapySDR.log(SoapySDR.LogLevel.NOTICE, "NOTICE")

    SoapySDR.restoreDefaultLogger()
    SoapySDR.log(SoapySDR.LogLevel.NOTICE, "NOTICE")
