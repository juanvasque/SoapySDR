# Copyright (c) 2022 Nicholas Corgan
# SPDX-License-Identifier: BSL-1.0

function test_suite=TestSoapyTypes
    try % assignment of 'localfunctions' is necessary in Matlab >= 2016
        test_functions=localfunctions();
    catch % no problem; early Matlab versions can use initTestSuite fine
    end
    initTestSuite;

function testRange
    SoapySDR

    range = SoapySDR.getUnitTestRange()
    assertEqual(0.0, range.minimum)
    assertEqual(1.0, range.maximum)
    assertEqual(0.1, range.step)

function testRangeList
    SoapySDR

    rangeList = SoapySDR.getUnitTestRangeList()
    for i = 1:3
        assertEqual(0.0 * (i-1), rangeList{i,1}.minimum)
        assertEqual(1.0 * (i-1), rangeList{i,1}.maximum)
        assertEqual(0.1 * (i-1), rangeList{i,1}.step)
    endfor

function testArgInfo
    SoapySDR

    argInfo = SoapySDR.getUnitTestArgInfo()
    expectedKey = "key"
    expectedName = "name"
    expectedDescription = "description"
    expectedUnits = "units"
    expectedType = SoapySDR.ArgType.FLOAT
    expectedRange = SoapySDR.Range(0.0, 1.0, 0.1)
    expectedOptions = ["opt1"; "opt2"; "opt3"]
    expectedOptionNames = ["Option1"; "Option2"; "Option3"]

    assertEqual(expectedKey, argInfo.key)
    assertEqual(expectedName, argInfo.name)
    assertEqual(expectedDescription, argInfo.description)
    assertEqual(expectedUnits, argInfo.units)
    assertEqual(expectedType, argInfo.type)
    assertEqual(expectedKey, argInfo.key)
    assertEqual(expectedRange.minimum, argInfo.range.minimum)
    assertEqual(expectedRange.maximum, argInfo.range.maximum)
    assertEqual(expectedRange.step, argInfo.range.step)
    assertEqual(expectedOptions, argInfo.options)
    assertEqual(expectedOptionNames, argInfo.optionNames)

function testArgInfoList
    SoapySDR

    argInfoList = SoapySDR.getUnitTestArgInfoList()
    for i = 1:3
        expectedKey = sprintf("key%d", (i-1))
        expectedName = sprintf("name%d", (i-1))
        expectedDescription = sprintf("description%d", (i-1))
        expectedUnits = sprintf("units%d", (i-1))
        expectedType = SoapySDR.ArgType.FLOAT
        expectedRange = SoapySDR.Range(0.0 * (i-1), 1.0 * (i-1), 0.1 * (i-1))
        expectedOptions = [sprintf("opt1_%d", (i-1)); sprintf("opt2_%d", (i-1)); sprintf("opt3_%d", (i-1))]
        expectedOptionNames = [sprintf("Option1_%d", (i-1)); sprintf("Option2_%d", (i-1)); sprintf("Option3_%d", (i-1))]

        assertEqual(expectedKey, argInfoList{i,1}.key)
        assertEqual(expectedName, argInfoList{i,1}.name)
        assertEqual(expectedDescription, argInfoList{i,1}.description)
        assertEqual(expectedUnits, argInfoList{i,1}.units)
        assertEqual(expectedType, argInfoList{i,1}.type)
        assertEqual(expectedKey, argInfoList{i,1}.key)
        assertEqual(expectedRange.minimum, argInfoList{i,1}.range.minimum)
        assertEqual(expectedRange.maximum, argInfoList{i,1}.range.maximum)
        assertEqual(expectedRange.step, argInfoList{i,1}.range.step)
        assertEqual(expectedOptions, argInfoList{i,1}.options)
        assertEqual(expectedOptionNames, argInfoList{i,1}.optionNames)
    endfor
