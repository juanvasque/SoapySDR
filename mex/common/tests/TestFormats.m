# Copyright (c) 2022 Nicholas Corgan
# SPDX-License-Identifier: BSL-1.0

function test_suite=TestFormat
    try % assignment of 'localfunctions' is necessary in Matlab >= 2016
        test_functions=localfunctions();
    catch % no problem; early Matlab versions can use initTestSuite fine
    end
    initTestSuite;

function testFormatSizes
    SoapySDR;

    assertEqual(1, SoapySDR.StreamFormat.toSize(SoapySDR.StreamFormat.U8));
    assertEqual(1, SoapySDR.StreamFormat.toSize(SoapySDR.StreamFormat.S8));
    assertEqual(2, SoapySDR.StreamFormat.toSize(SoapySDR.StreamFormat.U16));
    assertEqual(2, SoapySDR.StreamFormat.toSize(SoapySDR.StreamFormat.S16));
    assertEqual(4, SoapySDR.StreamFormat.toSize(SoapySDR.StreamFormat.U32));
    assertEqual(4, SoapySDR.StreamFormat.toSize(SoapySDR.StreamFormat.S32));
    assertEqual(4, SoapySDR.StreamFormat.toSize(SoapySDR.StreamFormat.F32));
    assertEqual(8, SoapySDR.StreamFormat.toSize(SoapySDR.StreamFormat.F64));

    assertEqual(2, SoapySDR.StreamFormat.toSize(SoapySDR.StreamFormat.CU8));
    assertEqual(2, SoapySDR.StreamFormat.toSize(SoapySDR.StreamFormat.CS8));
    assertEqual(4, SoapySDR.StreamFormat.toSize(SoapySDR.StreamFormat.CU16));
    assertEqual(4, SoapySDR.StreamFormat.toSize(SoapySDR.StreamFormat.CS16));
    assertEqual(8, SoapySDR.StreamFormat.toSize(SoapySDR.StreamFormat.CU32));
    assertEqual(8, SoapySDR.StreamFormat.toSize(SoapySDR.StreamFormat.CS32));
    assertEqual(8, SoapySDR.StreamFormat.toSize(SoapySDR.StreamFormat.CF32));
    assertEqual(16, SoapySDR.StreamFormat.toSize(SoapySDR.StreamFormat.CF64));
