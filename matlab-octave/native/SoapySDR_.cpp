// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#include "mexplus.h"

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Errors.hpp>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Logger.hpp>
#include <SoapySDR/Time.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Version.hpp>

#include <algorithm>
#include <iterator>
#include <vector>

using namespace mexplus;

//
// <SoapySDR/Device.hpp>
//

MEX_DEFINE(Device_Enumerate) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    InputArguments input(nrhs, prhs, 1);
    OutputArguments output(nlhs, plhs, 1);

    const auto args = input.get<std::string>(0);
    const auto devicesKwargs = SoapySDR::Device::enumerate(args);

    std::vector<std::string> devices;
    std::transform(
        devicesKwargs.begin(),
        devicesKwargs.end(),
        std::back_inserter(devices),
        SoapySDR::KwargsToString);

    output.set(0, devices);
}

//
// <SoapySDR/Errors.hpp>
//

//
// <SoapySDR/Formats.hpp>
//

//
// <SoapySDR/Logger.hpp>
//

//
// <SoapySDR/Time.hpp>
//

MEX_DEFINE(Time_ticksToTimeNs) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    InputArguments input(nrhs, prhs, 2);
    OutputArguments output(nlhs, plhs, 1);

    output.set(
        0,
        SoapySDR::ticksToTimeNs(
            input.get<long long>(0),
            input.get<double>(1)));
}

MEX_DEFINE(Time_timeNsToTicks) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    InputArguments input(nrhs, prhs, 2);
    OutputArguments output(nlhs, plhs, 1);

    output.set(
        0,
        SoapySDR::timeNsToTicks(
            input.get<long long>(0),
            input.get<double>(1)));
}

//
// <SoapySDR/Types.hpp>
//

//
// <SoapySDR/Version.hpp>
//
// TODO: ABI check
//

MEX_DEFINE(Version_GetAPIVersion) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    OutputArguments output(nlhs, plhs, 1);
    output.set(0, SoapySDR::getAPIVersion());
}

MEX_DEFINE(Version_GetABIVersion) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    OutputArguments output(nlhs, plhs, 1);
    output.set(0, SoapySDR::getABIVersion());
}

MEX_DEFINE(Version_GetLibVersion) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    OutputArguments output(nlhs, plhs, 1);
    output.set(0, SoapySDR::getLibVersion());
}

//
// mexplus
//

MEX_DISPATCH
