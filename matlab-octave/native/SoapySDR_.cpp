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
// Utility
//

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

template <>
inline mxArray *MxArray::from(const SoapySDR::ArgInfo::Type &type)
{
    return MxArray::from(int(type));
}

template <>
inline mxArray *MxArray::from(const SoapySDRArgInfoType &type)
{
    return MxArray::from(int(type));
}

// For simplicity, use strings in this layer
template <>
inline mxArray *MxArray::from(const SoapySDR::Kwargs &args)
{
    return MxArray::from(SoapySDR::KwargsToString(args));
}

template <typename T>
static inline void setGlobalVariable(const std::string &name, const T &value)
{
    mexPutVariable("global", name.c_str(), MxArray::from(value));
}

struct GlobalVarInit
{
    GlobalVarInit(void)
    {
        #define SET_GLOBAL_VAR(x) setGlobalVariable(#x, x)

        // <SoapySDR/Errors.hpp>
        SET_GLOBAL_VAR(SOAPY_SDR_TIMEOUT);
        SET_GLOBAL_VAR(SOAPY_SDR_STREAM_ERROR);
        SET_GLOBAL_VAR(SOAPY_SDR_CORRUPTION);
        SET_GLOBAL_VAR(SOAPY_SDR_OVERFLOW);
        SET_GLOBAL_VAR(SOAPY_SDR_NOT_SUPPORTED);
        SET_GLOBAL_VAR(SOAPY_SDR_TIME_ERROR);
        SET_GLOBAL_VAR(SOAPY_SDR_UNDERFLOW);

        // <SoapySDR/Formats.hpp>
        SET_GLOBAL_VAR(SOAPY_SDR_CF64);
        SET_GLOBAL_VAR(SOAPY_SDR_CF32);
        SET_GLOBAL_VAR(SOAPY_SDR_CU32);
        SET_GLOBAL_VAR(SOAPY_SDR_CU16);
        SET_GLOBAL_VAR(SOAPY_SDR_CU8);
        SET_GLOBAL_VAR(SOAPY_SDR_CS32);
        SET_GLOBAL_VAR(SOAPY_SDR_CS16);
        SET_GLOBAL_VAR(SOAPY_SDR_CS8);

        // <SoapySDR/Types.hpp>
        SET_GLOBAL_VAR(SOAPY_SDR_ARG_INFO_BOOL);
        SET_GLOBAL_VAR(SOAPY_SDR_ARG_INFO_INT);
        SET_GLOBAL_VAR(SOAPY_SDR_ARG_INFO_FLOAT);
        SET_GLOBAL_VAR(SOAPY_SDR_ARG_INFO_STRING);

        // <SoapySDR/Version.hpp>
        setGlobalVariable("SOAPY_SDR_API_VERSION", SoapySDR::getAPIVersion());
        setGlobalVariable("SOAPY_SDR_ABI_VERSION", SoapySDR::getABIVersion());
        setGlobalVariable("SOAPY_SDR_LIB_VERSION", SoapySDR::getLibVersion());

        #undef SET_GLOBAL_VAR
    }
};

static const GlobalVarInit globalVarInit;

//
// <SoapySDR/Device.hpp>
//

MEX_DEFINE(Device_enumerate) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    InputArguments input(nrhs, prhs, 1);
    OutputArguments output(nlhs, plhs, 1);

    output.set(0, SoapySDR::Device::enumerate(input.get<std::string>(0)));
}

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

template <>
mxArray *MxArray::from(const SoapySDR::Range &range)
{
    const char *fields[]
    {
        "minimum",
        "maximum",
        "step"
    };

    MxArray struct_array(MxArray::Struct(ARRAY_SIZE(fields), fields));

    #define SET_FIELD(x) struct_array.set(#x, range.x())
    SET_FIELD(minimum);
    SET_FIELD(maximum);
    SET_FIELD(step);
    #undef SET_FIELD

    return struct_array.release();
}

template <>
mxArray *MxArray::from(const SoapySDR::ArgInfo &argInfo)
{
    const char *fields[]
    {
        "key",
        "value",
        "name",
        "description",
        "units",
        "type",
        "range",
        "options",
        "optionNames",
    };

    MxArray struct_array(MxArray::Struct(ARRAY_SIZE(fields), fields));

    #define SET_FIELD(x) struct_array.set(#x, argInfo.x);
    SET_FIELD(key);
    SET_FIELD(value);
    SET_FIELD(name);
    SET_FIELD(description);
    SET_FIELD(units);
    SET_FIELD(type);
    SET_FIELD(range);
    SET_FIELD(options);
    SET_FIELD(optionNames);
    #undef SET_FIELD

    return struct_array.release();
}

//
// mexplus
//

MEX_DISPATCH
