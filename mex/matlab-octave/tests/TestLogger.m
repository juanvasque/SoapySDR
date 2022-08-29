# Copyright (c) 2022 Nicholas Corgan
# SPDX-License-Identifier: BSL-1.0

function test_suite=TestLogger
    try % assignment of 'localfunctions' is necessary in Matlab >= 2016;
        test_functions=localfunctions();
    catch % no problem; early Matlab versions can use initTestSuite fine;
    end;
    initTestSuite;

function fcn = loggerFcn(level, message)
    printf("Global function: %d: %s\n", level, message);

function testLog
    SoapySDR_setLogLevel(SoapySDR_LogLevel.Trace);
    # TODO: test getter when implemented;

    SoapySDR_log(SoapySDR_LogLevel.Fatal, "Fatal");
    SoapySDR_log(SoapySDR_LogLevel.Critical, "Critical");
    SoapySDR_log(SoapySDR_LogLevel.Error, "Error");
    SoapySDR_log(SoapySDR_LogLevel.Warning, "Warning");
    SoapySDR_log(SoapySDR_LogLevel.Notice, "Notice");
    SoapySDR_log(SoapySDR_LogLevel.Info, "Info");
    SoapySDR_log(SoapySDR_LogLevel.Debug, "Debug");
    SoapySDR_log(SoapySDR_LogLevel.Trace, "Trace");
    SoapySDR_log(SoapySDR_LogLevel.SSI, "SSI");
    puts("\n") # For the sake of formatting;

    SoapySDR_setLogLevel(SoapySDR_LogLevel.Notice);
    SoapySDR_logf(SoapySDR_LogLevel.Notice, "Notice: %s %d %f", "str", 1351, 4.18);

    % Normal function
    SoapySDR_registerLogHandler(@loggerFcn);
    SoapySDR_log(SoapySDR_LogLevel.Notice, "Notice");
    SoapySDR_logf(SoapySDR_LogLevel.Notice, "Notice: %s %d %f", "str", 1351, 4.18);

    % Anonymous function
    SoapySDR_registerLogHandler(@(level, message) printf("Anonymous function: %d: %s\n", level, message));
    SoapySDR_log(SoapySDR_LogLevel.Notice, "Notice");
    SoapySDR_logf(SoapySDR_LogLevel.Notice, "Notice: %s %d %f", "str", 1351, 4.18);

    SoapySDR_clearLogHandler();
    SoapySDR_log(SoapySDR_LogLevel.Notice, "Notice");
    SoapySDR_logf(SoapySDR_LogLevel.Notice, "Notice: %s %d %f", "str", 1351, 4.18);

% For whatever reason, the unit test infrastructure isn't catching errors inside the
% log handlers. Uncomment this out to manually run it.
%
% Make sure if a Matlab/Octave error occurs inside the log function,
% everything is cleaned up nicely.
% function testErrorInLogHandler
%     SoapySDR_setLogLevel(SoapySDR_LogLevel.Notice);
%
%     % Explicit error
%     SoapySDR_registerLogHandler(@(level, message) error(sprintf("%d: %s", level, message)));
%
%     % TODO: why isn't assertExceptionThrown catching these?
%     SoapySDR_log(SoapySDR_LogLevel.Notice, "This is an error");
%     SoapySDR_logf(SoapySDR_LogLevel.Notice, "This is an error: %d", 12345);
