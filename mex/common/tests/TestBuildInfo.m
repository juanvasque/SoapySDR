# Copyright (c) 2022 Nicholas Corgan
# SPDX-License-Identifier: BSL-1.0

function test_suite=TestBuildInfo
    try % assignment of 'localfunctions' is necessary in Matlab >= 2016
        test_functions=localfunctions();
    catch % no problem; early Matlab versions can use initTestSuite fine
    end
    initTestSuite;

function testBuildInfoStrings
    SoapySDR;

    assertNotEqual("", SoapySDR.getABIVersion());
    assertNotEqual("", SoapySDR.getAPIVersion());
    assertNotEqual("", SoapySDR.getLibVersion());
