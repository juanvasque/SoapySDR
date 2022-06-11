// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

using NUnit.Framework;

[TestFixture]
public class TestBuildInfo
{
    [Test]
    public void Test_BuildInfoStrings()
    {
        Assert.IsNotEmpty(SoapySDR.BuildInfo.Assembly.ABIVersion);
        Assert.IsNotEmpty(SoapySDR.BuildInfo.Assembly.APIVersion);
        Assert.IsNotEmpty(SoapySDR.BuildInfo.Assembly.LibVersion);
        System.Console.WriteLine("Assembly: ABI={0}, API={1}, Lib={2}", SoapySDR.BuildInfo.Assembly.ABIVersion, SoapySDR.BuildInfo.Assembly.APIVersion, SoapySDR.BuildInfo.Assembly.LibVersion);

        Assert.IsNotEmpty(SoapySDR.BuildInfo.SWIGModule.ABIVersion);
        Assert.IsNotEmpty(SoapySDR.BuildInfo.SWIGModule.APIVersion);
        Assert.IsNotEmpty(SoapySDR.BuildInfo.SWIGModule.LibVersion);
        System.Console.WriteLine("SWIGModule: ABI={0}, API={1}, Lib={2}", SoapySDR.BuildInfo.SWIGModule.ABIVersion, SoapySDR.BuildInfo.SWIGModule.APIVersion, SoapySDR.BuildInfo.SWIGModule.LibVersion);

        Assert.IsNotEmpty(SoapySDR.BuildInfo.Runtime.ABIVersion);
        Assert.IsNotEmpty(SoapySDR.BuildInfo.Runtime.APIVersion);
        Assert.IsNotEmpty(SoapySDR.BuildInfo.Runtime.LibVersion);
        System.Console.WriteLine("Runtime: ABI={0}, API={1}, Lib={2}", SoapySDR.BuildInfo.Runtime.ABIVersion, SoapySDR.BuildInfo.Runtime.APIVersion, SoapySDR.BuildInfo.Runtime.LibVersion);
    }

    public static int Main(string[] args) => TestRunner.RunNUnitTest("TestBuildInfo");
}