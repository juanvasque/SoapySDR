# Copyright (c) 2022 Nicholas Corgan
# SPDX-License-Identifier: BSL-1.0

function test_suite=TestErrorCodes
    try % assignment of 'localfunctions' is necessary in Matlab >= 2016
        test_functions=localfunctions();
    catch % no problem; early Matlab versions can use initTestSuite fine
    end
    initTestSuite;

function testErrorCodes
    assertEqual("TIMEOUT", SoapySDR_ErrorCode.toString(SoapySDR_ErrorCode.Timeout));
    assertEqual("STREAM_ERROR", SoapySDR_ErrorCode.toString(SoapySDR_ErrorCode.StreamError));
    assertEqual("CORRUPTION", SoapySDR_ErrorCode.toString(SoapySDR_ErrorCode.Corruption));
    assertEqual("OVERFLOW", SoapySDR_ErrorCode.toString(SoapySDR_ErrorCode.Overflow));
    assertEqual("NOT_SUPPORTED", SoapySDR_ErrorCode.toString(SoapySDR_ErrorCode.NotSupported));
    assertEqual("TIME_ERROR", SoapySDR_ErrorCode.toString(SoapySDR_ErrorCode.TimeError));
    assertEqual("UNDERFLOW", SoapySDR_ErrorCode.toString(SoapySDR_ErrorCode.Underflow));
    assertEqual("UNKNOWN", SoapySDR_ErrorCode.toString(0));
