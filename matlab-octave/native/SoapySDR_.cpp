// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#include "mexplus.h"

#include <SoapySDR/Constants.h>
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

template <typename T>
static inline uintptr_t fromPointer(T *ptr)
{
    return reinterpret_cast<uintptr_t>(ptr);
}

template <typename T>
static inline T * toPointer(uintptr_t num)
{
    return reinterpret_cast<T *>(num);
}

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
inline void setGlobalVariable(const std::string &name, const T &value)
{
    mexPutVariable("global", name.c_str(), MxArray::from(value));
}

struct GlobalVarInit
{
    GlobalVarInit(void)
    {
        #define SET_GLOBAL_VAR(x) setGlobalVariable(#x, x)

        // <SoapySDR/Constants.h>
        SET_GLOBAL_VAR(SOAPY_SDR_TX);
        SET_GLOBAL_VAR(SOAPY_SDR_RX);
        SET_GLOBAL_VAR(SOAPY_SDR_END_BURST);
        SET_GLOBAL_VAR(SOAPY_SDR_HAS_TIME);
        SET_GLOBAL_VAR(SOAPY_SDR_END_ABRUPT);
        SET_GLOBAL_VAR(SOAPY_SDR_ONE_PACKET);
        SET_GLOBAL_VAR(SOAPY_SDR_MORE_FRAGMENTS);
        SET_GLOBAL_VAR(SOAPY_SDR_WAIT_TRIGGER);
        SET_GLOBAL_VAR(SOAPY_SDR_USER_FLAG0);
        SET_GLOBAL_VAR(SOAPY_SDR_USER_FLAG1);
        SET_GLOBAL_VAR(SOAPY_SDR_USER_FLAG2);
        SET_GLOBAL_VAR(SOAPY_SDR_USER_FLAG3);
        SET_GLOBAL_VAR(SOAPY_SDR_USER_FLAG4);

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
// <SoapySDR/Device.hpp>
//

template <>
void MxArray::to(const mxArray *array, SoapySDR::Device **device)
{
    if(!device)
        mexErrMsgTxt("Null pointer exception");

    uintptr_t num;
    MxArray::to(array, &num);

    *device = toPointer<SoapySDR::Device>(num);
}

MEX_DEFINE(Device_enumerate) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    InputArguments input(nrhs, prhs, 1);
    OutputArguments output(nlhs, plhs, 1);

    output.set(0, SoapySDR::Device::enumerate(input.get<std::string>(0)));
}

MEX_DEFINE(Device_make) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    InputArguments input(nrhs, prhs, 1);
    OutputArguments output(nlhs, plhs, 1);

    // For some reason, we can't specialize MxArray::from<> for pointers, so
    // we need this extra shim.
    output.set(0, fromPointer(SoapySDR::Device::make(input.get<std::string>(0))));
}

MEX_DEFINE(Device_unmake) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    InputArguments input(nrhs, prhs, 1);

    SoapySDR::Device::unmake(input.get<SoapySDR::Device *>(0));
}

MEX_DEFINE(Device_getDriverKey) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    InputArguments input(nrhs, prhs, 1);
    OutputArguments output(nlhs, plhs, 1);

    output.set(0, input.get<SoapySDR::Device *>(0)->getDriverKey());
}

MEX_DEFINE(Device_getHardwareKey) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    InputArguments input(nrhs, prhs, 1);
    OutputArguments output(nlhs, plhs, 1);

    output.set(0, input.get<SoapySDR::Device *>(0)->getHardwareKey());
}

MEX_DEFINE(Device_getHardwareInfo) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    InputArguments input(nrhs, prhs, 1);
    OutputArguments output(nlhs, plhs, 1);

    output.set(0, input.get<SoapySDR::Device *>(0)->getHardwareInfo());
}

MEX_DEFINE(Device_setFrontendMapping) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    InputArguments input(nrhs, prhs, 3);

    input.get<SoapySDR::Device *>(0)->setFrontendMapping(
        input.get<int>(1),
        input.get<std::string>(2));
}

MEX_DEFINE(Device_getFrontendMapping) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    InputArguments input(nrhs, prhs, 2);
    OutputArguments output(nlhs, plhs, 1);

    output.set(
        0,
        input.get<SoapySDR::Device *>(0)->getFrontendMapping(
            input.get<int>(1)));
}

MEX_DEFINE(Device_getNumChannels) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    InputArguments input(nrhs, prhs, 2);
    OutputArguments output(nlhs, plhs, 1);

    output.set(
        0,
        input.get<SoapySDR::Device *>(0)->getNumChannels(
            input.get<int>(1)));
}

//
// mexplus
//

MEX_DISPATCH
