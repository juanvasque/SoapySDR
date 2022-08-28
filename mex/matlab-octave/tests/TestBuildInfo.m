# Copyright (c) 2022 Nicholas Corgan
# SPDX-License-Identifier: BSL-1.0

function test_suite=TestBuildInfo
    try % assignment of 'localfunctions' is necessary in Matlab >= 2016
        test_functions=localfunctions();
    catch % no problem; early Matlab versions can use initTestSuite fine
    end
    initTestSuite;

function testBuildInfoStrings
    assertLessThan(0, length(SoapySDR_getABIVersion()));
    assertLessThan(0, length(SoapySDR_getMEXABIVersion()));
    assertLessThan(0, length(SoapySDR_getScriptABIVersion()));
    assertLessThan(0, length(SoapySDR_getAPIVersion()));
    assertLessThan(0, length(SoapySDR_getLibVersion()));
