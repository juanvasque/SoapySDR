# Copyright (c) 2022 Nicholas Corgan
# SPDX-License-Identifier: BSL-1.0

function test_suite=TestErrorCodes
    try % assignment of 'localfunctions' is necessary in Matlab >= 2016
        test_functions=localfunctions();
    catch % no problem; early Matlab versions can use initTestSuite fine
    end
    initTestSuite;

function testErrorCodes
    SoapySDR;

    assertEqual("TIMEOUT", SoapySDR.Error.toString(SoapySDR.Error.TIMEOUT));
    assertEqual("STREAM_ERROR", SoapySDR.Error.toString(SoapySDR.Error.STREAM_ERROR));
    assertEqual("CORRUPTION", SoapySDR.Error.toString(SoapySDR.Error.CORRUPTION));
    assertEqual("OVERFLOW", SoapySDR.Error.toString(SoapySDR.Error.OVERFLOW_ERROR));
    assertEqual("NOT_SUPPORTED", SoapySDR.Error.toString(SoapySDR.Error.NOT_SUPPORTED));
    assertEqual("TIME_ERROR", SoapySDR.Error.toString(SoapySDR.Error.TIME_ERROR));
    assertEqual("UNDERFLOW", SoapySDR.Error.toString(SoapySDR.Error.UNDERFLOW_ERROR));
    assertEqual("UNKNOWN", SoapySDR.Error.toString(0));
