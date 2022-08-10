# Copyright (c) 2022 Nicholas Corgan
# SPDX-License-Identifier: BSL-1.0

function test_suite=TestConstants
    try % assignment of 'localfunctions' is necessary in Matlab >= 2016
        test_functions=localfunctions();
    catch % no problem; early Matlab versions can use initTestSuite fine
    end
    initTestSuite;

function testArgInfoType
    SoapySDR_ArgInfoType.Bool;
    SoapySDR_ArgInfoType.Int;
    SoapySDR_ArgInfoType.Float;
    SoapySDR_ArgInfoType.String;

function testDirection
    SoapySDR_Direction.Tx;
    SoapySDR_Direction.Rx;

function testStreamFlag
    SoapySDR_StreamFlag.EndBurst;
    SoapySDR_StreamFlag.HasTime;
    SoapySDR_StreamFlag.EndAbrupt;
    SoapySDR_StreamFlag.OnePacket;
    SoapySDR_StreamFlag.MoreFragments;
    SoapySDR_StreamFlag.WaitTrigger;
    SoapySDR_StreamFlag.UserFlag0;
    SoapySDR_StreamFlag.UserFlag1;
    SoapySDR_StreamFlag.UserFlag2;
    SoapySDR_StreamFlag.UserFlag3;
    SoapySDR_StreamFlag.UserFlag4;

function testStreamFormat
    assertEqual("CS8", SoapySDR_StreamFormat.CS8);
    assertEqual("CS16", SoapySDR_StreamFormat.CS16);
    assertEqual("CS32", SoapySDR_StreamFormat.CS32);
    assertEqual("CU8", SoapySDR_StreamFormat.CU8);
    assertEqual("CU16", SoapySDR_StreamFormat.CU16);
    assertEqual("CU32", SoapySDR_StreamFormat.CU32);
    assertEqual("CF32", SoapySDR_StreamFormat.CF32);
    assertEqual("CF64", SoapySDR_StreamFormat.CF64);
