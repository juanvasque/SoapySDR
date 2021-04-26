// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%insert(runtime) %{

#include "CSharpExtensions.hpp"

#include <SoapySDR/Logger.hpp>

typedef void(SWIGSTDCALL* SoapySDRCSharpLogHandler)(int logLevel, const std::string& message);

static SoapySDRCSharpLogHandler CSharpLogHandler = nullptr;

extern "C"
{
    SWIGEXPORT void SWIGSTDCALL RegisterSoapySDRCSharpLogHandler(SoapySDRCSharpLogHandler logHandler)
    {
        CSharpLogHandler = logHandler;
    }
}

%}

%pragma(csharp) imclasscode=%{
    internal class SoapySDRLogHelper
    {
        [global::System.Runtime.InteropServices.DllImport("$dllimport", EntryPoint="RegisterSoapySDRCSharpLogHandler")]
        private public static extern void RegisterSoapySDRCSharpLogHandler(SoapySDRLogDelegate logDelegate);

        private static void NativeLogCaller(int logLevel, string message)
        {
            logDelegate(LogLevel(logLevel), message);
        }

        private delegate NativeSoapySDRLogDelegate(int logLevel, string message);
        private static NativeSoapySDRLogDelegate nativeLogDelegate = new NativeSoapySDRLogDelegate(NativeLogCaller);

        public delegate SoapySDRLogDelegate(LogLevel logLevel, string message);
        static SoapySDRLogDelegate logDelegate = new SoapySDRLogDelegate(DefaultLog);

        private static void DefaultLog(LogLevel logLevel, string message)
        {
            System.Console.WriteLine(logLevel.ToString() + ": " + message);
        }

        public static void RegisterLogHandler(SoapySDRLogDelegate logDelegate)
        {
            logDelegate = del;
        }

        static SoapySDRLogHelper()
        {
            RegisterSoapySDRCSharpLogHandler(nativeLogDelegate);
        }
    };
%}