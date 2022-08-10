# Copyright (c) 2022 Nicholas Corgan
# SPDX-License-Identifier: BSL-1.0

function test_suite=TestEnumerateDevices
    try % assignment of 'localfunctions' is necessary in Matlab >= 2016
        test_functions=localfunctions();
    catch % no problem; early Matlab versions can use initTestSuite fine
    end
    initTestSuite;

function testEnumerateNoParams
    # Just make sure this runs, no idea what's actually there
    SoapySDR_Device.enumerate();

function testEnumerateWithParams
    # TODO: check output for null device
    deviceStringArgs = SoapySDR_Device.enumerate("type=null");
