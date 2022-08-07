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

//////////////////////////////////////////////////////
// Utility
//////////////////////////////////////////////////////

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

template <typename Fcn>
static void safeCall(const Fcn &fcn, const std::string &context)
{
    try { fcn(); }
    catch(const std::exception &ex)
    {
        mexErrMsgIdAndTxt(context.c_str(), ex.what());
    }
    catch(...)
    {
        mexErrMsgIdAndTxt(context.c_str(), "Unknown error");
    }
}

//////////////////////////////////////////////////////
// <SoapySDR/Logger.hpp>
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////
// <SoapySDR/Time.hpp>
//////////////////////////////////////////////////////

MEX_DEFINE(Time_ticksToTimeNs) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                SoapySDR::ticksToTimeNs(
                    input.get<long long>(0),
                    input.get<double>(1)));
        },
        "ticksToTimeNs");
}

MEX_DEFINE(Time_timeNsToTicks) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                SoapySDR::timeNsToTicks(
                    input.get<long long>(0),
                    input.get<double>(1)));
        },
        "timeNsToTicks");
}

//////////////////////////////////////////////////////
// <SoapySDR/Types.hpp>
//////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////
// <SoapySDR/Device.hpp>
//////////////////////////////////////////////////////

//
// Device helper struct
//

// Since we can't specialize from() for pointers
struct DeviceContainer
{
    SoapySDR::Device *ptr{nullptr};

    DeviceContainer(void) = default;
    DeviceContainer(SoapySDR::Device *ptr_): ptr(ptr_){}
};

template <>
mxArray *MxArray::from(const DeviceContainer &device)
{
    const char *fields[] = {"driverKey", "hardwareKey", "hardwareInfo", "__internal"};
    MxArray struct_array(MxArray::Struct(ARRAY_SIZE(fields), fields));

    struct_array.set("driverKey", device.ptr->getDriverKey());
    struct_array.set("hardwareKey", device.ptr->getHardwareKey());
    struct_array.set("hardwareInfo", device.ptr->getHardwareInfo());
    struct_array.set("__internal", reinterpret_cast<uintptr_t>(device.ptr));

    return struct_array.release();
}

template <>
void MxArray::to(const mxArray *array, DeviceContainer *device)
{
    if(!device)
        mexErrMsgTxt("Null pointer exception");

    uintptr_t num;
    MxArray::at(array, "__internal", &num);

    if(!num)
        mexErrMsgTxt("Null pointer exception");

    device->ptr = reinterpret_cast<SoapySDR::Device *>(num);
}

//
// Stream helper struct
//

struct StreamContainer
{
    SoapySDR::Stream *stream{nullptr};
    SoapySDR::Device *device{nullptr};

    std::string format;
    std::vector<size_t> channels;
    std::string args;
};

//
// Enumeration
//

MEX_DEFINE(Device_enumerate) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);
            OutputArguments output(nlhs, plhs, 1);

            output.set(0, SoapySDR::Device::enumerate(input.get<std::string>(0)));
        },
        "Device_enumerate");
}

//
// Construction/destruction API
//

MEX_DEFINE(Device_make) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);
            OutputArguments output(nlhs, plhs, 1);

            output.set(0, DeviceContainer(SoapySDR::Device::make(input.get<std::string>(0))));
        },
        "Device_make");
}

MEX_DEFINE(Device_unmake) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);

            SoapySDR::Device::unmake(input.get<DeviceContainer>(0).ptr);
        },
        "Device_unmake");
}

//
// Channels API
//

MEX_DEFINE(Device_setFrontendMapping) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);

            input.get<DeviceContainer>(0).ptr->setFrontendMapping(
                input.get<int>(1),
                input.get<std::string>(2));
        },
        "Device_setFrontendMapping");
}

MEX_DEFINE(Device_getFrontendMapping) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getFrontendMapping(
                    input.get<int>(1)));
        },
        "Device_getFrontendMapping");
}

MEX_DEFINE(Device_getNumChannels) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getNumChannels(
                    input.get<int>(1)));
        },
        "Device_getNumChannels");
}

MEX_DEFINE(Device_getChannelInfo) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getChannelInfo(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getChannelInfo");
}

MEX_DEFINE(Device_getFullDuplex) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getFullDuplex(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getFullDuplex");
}

//
// Stream API
//

MEX_DEFINE(Device_getStreamFormats) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getStreamFormats(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getStreamFormats");
}

MEX_DEFINE(Device_getNativeStreamFormat) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 2);

            double fullScale;
            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getNativeStreamFormat(
                    input.get<int>(1),
                    input.get<size_t>(2),
                    fullScale));
            output.set(1, fullScale);
        },
        "Device_getNativeStreamFormat");
}

MEX_DEFINE(Device_getStreamArgsInfo) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getStreamArgsInfo(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getStreamArgsInfo");
}

//
// Antenna API
//

MEX_DEFINE(Device_listAntennas) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->listAntennas(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_listAntennas");
}

MEX_DEFINE(Device_setAntenna) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);

            input.get<DeviceContainer>(0).ptr->setAntenna(
                input.get<int>(1),
                input.get<size_t>(2),
                input.get<std::string>(3));
        },
        "Device_setAntenna");
}

MEX_DEFINE(Device_getAntenna) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getAntenna(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getAntenna");
}

//
// Frontend corrections API
//

MEX_DEFINE(Device_hasDCOffsetMode) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->hasDCOffsetMode(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_hasDCOffset");
}

MEX_DEFINE(Device_setDCOffsetMode) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);

            input.get<DeviceContainer>(0).ptr->setDCOffsetMode(
                input.get<int>(1),
                input.get<size_t>(2),
                input.get<bool>(3));
        },
        "Device_setDCOffsetMode");
}

MEX_DEFINE(Device_getDCOffsetMode) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getDCOffsetMode(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getDCOffsetMode");
}

MEX_DEFINE(Device_hasDCOffset) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->hasDCOffset(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_hasDCOffset");
}

MEX_DEFINE(Device_setDCOffset) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);

            input.get<DeviceContainer>(0).ptr->setDCOffset(
                input.get<int>(1),
                input.get<size_t>(2),
                input.get<std::complex<double>>(3));
        },
        "Device_setDCOffset");
}

MEX_DEFINE(Device_getDCOffset) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getDCOffset(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getDCOffset");
}

MEX_DEFINE(Device_hasIQBalanceMode) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->hasIQBalanceMode(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_hasIQBalance");
}

MEX_DEFINE(Device_setIQBalanceMode) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);

            input.get<DeviceContainer>(0).ptr->setIQBalanceMode(
                input.get<int>(1),
                input.get<size_t>(2),
                input.get<bool>(3));
        },
        "Device_setIQBalanceMode");
}

MEX_DEFINE(Device_getIQBalanceMode) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getIQBalanceMode(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getIQBalanceMode");
}

MEX_DEFINE(Device_hasIQBalance) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->hasIQBalance(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_hasIQBalance");
}

MEX_DEFINE(Device_setIQBalance) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);

            input.get<DeviceContainer>(0).ptr->setIQBalance(
                input.get<int>(1),
                input.get<size_t>(2),
                input.get<std::complex<double>>(3));
        },
        "Device_setIQBalance");
}

MEX_DEFINE(Device_getIQBalance) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getIQBalance(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getIQBalance");
}

MEX_DEFINE(Device_hasFrequencyCorrection) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->hasFrequencyCorrection(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_hasFrequencyCorrection");
}

MEX_DEFINE(Device_setFrequencyCorrection) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);

            input.get<DeviceContainer>(0).ptr->setFrequencyCorrection(
                input.get<int>(1),
                input.get<size_t>(2),
                input.get<double>(3));
        },
        "Device_setFrequencyCorrection");
}

MEX_DEFINE(Device_getFrequencyCorrection) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getFrequencyCorrection(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getFrequencyCorrection");
}

//
// Gain API
//

MEX_DEFINE(Device_listGains) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->listGains(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_listGains");
}

MEX_DEFINE(Device_hasGainMode) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->hasGainMode(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_hasGain");
}

MEX_DEFINE(Device_setGainMode) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);

            input.get<DeviceContainer>(0).ptr->setGainMode(
                input.get<int>(1),
                input.get<size_t>(2),
                input.get<bool>(3));
        },
        "Device_setGainMode");
}

MEX_DEFINE(Device_getGainMode) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getGainMode(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getGainMode");
}

MEX_DEFINE(Device_setGain) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);

            input.get<DeviceContainer>(0).ptr->setGain(
                input.get<int>(1),
                input.get<size_t>(2),
                input.get<double>(3));
        },
        "Device_setGain");
}

MEX_DEFINE(Device_setGainElement) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 5);

            input.get<DeviceContainer>(0).ptr->setGain(
                input.get<int>(1),
                input.get<size_t>(2),
                input.get<std::string>(3),
                input.get<double>(4));
        },
        "Device_setGainElement");
}

MEX_DEFINE(Device_getGain) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getGain(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getGain");
}

MEX_DEFINE(Device_getGainElement) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getGain(
                    input.get<int>(1),
                    input.get<size_t>(2),
                    input.get<std::string>(3)));
        },
        "Device_getGainElement");
}

MEX_DEFINE(Device_getGainRange) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getGainRange(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getGainRange");
}

MEX_DEFINE(Device_getGainElementRange) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getGainRange(
                    input.get<int>(1),
                    input.get<size_t>(2),
                    input.get<std::string>(3)));
        },
        "Device_getGainElementRange");
}

//
// Frequency API
//

MEX_DEFINE(Device_setFrequency) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 5);

            input.get<DeviceContainer>(0).ptr->setFrequency(
                input.get<int>(1),
                input.get<size_t>(2),
                input.get<double>(3),
                SoapySDR::KwargsFromString(input.get<std::string>(4)));
        },
        "Device_setFrequency");
}

MEX_DEFINE(Device_setFrequencyComponent) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 5);

            input.get<DeviceContainer>(0).ptr->setFrequency(
                input.get<int>(1),
                input.get<size_t>(2),
                input.get<std::string>(3),
                input.get<double>(4),
                SoapySDR::KwargsFromString(input.get<std::string>(5)));
        },
        "Device_setFrequencyComponent");
}

MEX_DEFINE(Device_getFrequency) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getFrequency(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getFrequency");
}

MEX_DEFINE(Device_getFrequencyComponent) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getFrequency(
                    input.get<int>(1),
                    input.get<size_t>(2),
                    input.get<std::string>(3)));
        },
        "Device_getFrequencyComponent");
}

MEX_DEFINE(Device_getFrequencyRange) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getFrequencyRange(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getFrequencyRange");
}

MEX_DEFINE(Device_getFrequencyComponentRange) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getFrequencyRange(
                    input.get<int>(1),
                    input.get<size_t>(2),
                    input.get<std::string>(3)));
        },
        "Device_getFrequencyComponentRange");
}

MEX_DEFINE(Device_getFrequencyArgsInfo) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getFrequencyArgsInfo(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getFrequencyArgsInfo");
}

//
// Sample Rate API
//

MEX_DEFINE(Device_setSampleRate) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);

            input.get<DeviceContainer>(0).ptr->setSampleRate(
                input.get<int>(1),
                input.get<size_t>(2),
                input.get<double>(3));
        },
        "Device_setSampleRate");
}

MEX_DEFINE(Device_getSampleRate) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getSampleRate(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getSampleRate");
}

MEX_DEFINE(Device_getSampleRateRange) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getSampleRateRange(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getSampleRateRange");
}

//
// Bandwidth API
//

MEX_DEFINE(Device_setBandwidth) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);

            input.get<DeviceContainer>(0).ptr->setBandwidth(
                input.get<int>(1),
                input.get<size_t>(2),
                input.get<double>(3));
        },
        "Device_setBandwidth");
}

MEX_DEFINE(Device_getBandwidth) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getBandwidth(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getBandwidth");
}

MEX_DEFINE(Device_getBandwidthRange) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getBandwidthRange(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getBandwidthRange");
}

//
// Clocking API
//

MEX_DEFINE(Device_setMasterClockRate) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);

            input.get<DeviceContainer>(0).ptr->setMasterClockRate(input.get<double>(1));
        },
        "Device_setMasterClockRate");
}

MEX_DEFINE(Device_getMasterClockRate) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getMasterClockRate());
        },
        "Device_setMasterClockRate");
}

MEX_DEFINE(Device_getMasterClockRates) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getMasterClockRates());
        },
        "Device_getMasterClockRates");
}

MEX_DEFINE(Device_setReferenceClockRate) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);

            input.get<DeviceContainer>(0).ptr->setReferenceClockRate(input.get<double>(1));
        },
        "Device_setReferenceClockRate");
}

MEX_DEFINE(Device_getReferenceClockRate) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getReferenceClockRate());
        },
        "Device_setReferenceClockRate");
}

MEX_DEFINE(Device_getReferenceClockRates) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getReferenceClockRates());
        },
        "Device_getReferenceClockRates");
}

MEX_DEFINE(Device_listClockSources) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->listClockSources());
        },
        "Device_listClockSources");
}

MEX_DEFINE(Device_setClockSource) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);

            input.get<DeviceContainer>(0).ptr->setClockSource(input.get<std::string>(1));
        },
        "Device_setClockSource");
}

MEX_DEFINE(Device_getClockSource) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getClockSource());
        },
        "Device_setClockSource");
}

//
// Time API
//

MEX_DEFINE(Device_listTimeSources) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->listTimeSources());
        },
        "Device_listTimeSources");
}

MEX_DEFINE(Device_setTimeSource) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);

            input.get<DeviceContainer>(0).ptr->setTimeSource(input.get<std::string>(1));
        },
        "Device_setTimeSource");
}

MEX_DEFINE(Device_getTimeSource) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getTimeSource());
        },
        "Device_setTimeSource");
}

MEX_DEFINE(Device_hasHardwareTime) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->hasHardwareTime(input.get<std::string>(1)));
        },
        "Device_hasHardwareTime");
}

MEX_DEFINE(Device_getHardwareTime) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getHardwareTime(input.get<std::string>(1)));
        },
        "Device_getHardwareTime");
}

MEX_DEFINE(Device_setHardwareTime) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);

            input.get<DeviceContainer>(0).ptr->setHardwareTime(
                input.get<long long>(1),
                input.get<std::string>(2));
        },
        "Device_setHardwareTime");
}

//
// Sensor API
//

MEX_DEFINE(Device_listSensors) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->listSensors());
        },
        "Device_listSensors");
}

MEX_DEFINE(Device_getSensorInfo) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getSensorInfo(input.get<std::string>(1)));
        },
        "Device_getSensorInfo");
}

MEX_DEFINE(Device_readSensor) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->readSensor(input.get<std::string>(1)));
        },
        "Device_readSensor");
}

MEX_DEFINE(Device_listChannelSensors) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->listSensors(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_listChannelSensors");
}

MEX_DEFINE(Device_getChannelSensorInfo) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getSensorInfo(
                    input.get<int>(1),
                    input.get<size_t>(2),
                    input.get<std::string>(3)));
        },
        "Device_getChannelSensorInfo");
}

MEX_DEFINE(Device_readChannelSensor) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->readSensor(
                    input.get<int>(1),
                    input.get<size_t>(2),
                    input.get<std::string>(3)));
        },
        "Device_readChannelSensor");
}

//
// Register API
//

MEX_DEFINE(Device_listRegisterInterfaces) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->listRegisterInterfaces());
        },
        "Device_listRegisterInterfaces");
}

MEX_DEFINE(Device_writeRegister) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);

            input.get<DeviceContainer>(0).ptr->writeRegister(
                input.get<std::string>(1),
                input.get<unsigned>(2),
                input.get<unsigned>(3));
        },
        "Device_writeRegister");
}

MEX_DEFINE(Device_readRegister) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->readRegister(
                    input.get<std::string>(1),
                    input.get<unsigned>(2)));
        },
        "Device_readRegister");
}

MEX_DEFINE(Device_writeRegisters) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);

            input.get<DeviceContainer>(0).ptr->writeRegisters(
                input.get<std::string>(1),
                input.get<unsigned>(2),
                input.get<std::vector<unsigned>>(3));
        },
        "Device_writeRegisters");
}

MEX_DEFINE(Device_readRegisters) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->readRegisters(
                    input.get<std::string>(1),
                    input.get<unsigned>(2),
                    input.get<size_t>(3)));
        },
        "Device_readRegisters");
}

//
// Settings API
//

MEX_DEFINE(Device_getSettingInfo) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getSettingInfo());
        },
        "Device_getSettingInfo");
}

MEX_DEFINE(Device_getSettingInfoWithKey) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getSettingInfo(input.get<std::string>(1)));
        },
        "Device_getSettingInfoWithKey");
}

MEX_DEFINE(Device_writeSetting) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);

            input.get<DeviceContainer>(0).ptr->writeSetting(
                input.get<std::string>(1),
                input.get<std::string>(2));
        },
        "Device_writeSetting");
}

MEX_DEFINE(Device_readSetting) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->readSetting(input.get<std::string>(1)));
        },
        "Device_readSetting");
}

MEX_DEFINE(Device_getChannelSettingInfo) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getSettingInfo(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_getChannelSettingInfo");
}

MEX_DEFINE(Device_getChannelSettingInfoWithKey) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->getSettingInfo(
                    input.get<int>(1),
                    input.get<size_t>(2),
                    input.get<std::string>(3)));
        },
        "Device_getChannelSettingInfo");
}

MEX_DEFINE(Device_writeChannelSetting) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 5);

            input.get<DeviceContainer>(0).ptr->writeSetting(
                input.get<int>(1),
                input.get<size_t>(2),
                input.get<std::string>(3),
                input.get<std::string>(4));
        },
        "Device_writeChannelSetting");
}

MEX_DEFINE(Device_readChannelSetting) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->readSetting(
                    input.get<int>(1),
                    input.get<size_t>(2),
                    input.get<std::string>(3)));
        },
        "Device_readChannelSetting");
}

//
// GPIO API
//

MEX_DEFINE(Device_listGPIOBanks) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->listGPIOBanks());
        },
        "Device_listGPIOBanks");
}

MEX_DEFINE(Device_writeGPIO) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);

            input.get<DeviceContainer>(0).ptr->writeGPIO(
                input.get<std::string>(1),
                input.get<unsigned>(2));
        },
        "Device_writeGPIO");
}

MEX_DEFINE(Device_writeGPIOMasked) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);

            input.get<DeviceContainer>(0).ptr->writeGPIO(
                input.get<std::string>(1),
                input.get<unsigned>(2),
                input.get<unsigned>(3));
        },
        "Device_writeGPIOMasked");
}

MEX_DEFINE(Device_readGPIO) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->readGPIO(input.get<std::string>(1)));
        },
        "Device_readGPIO");
}

MEX_DEFINE(Device_writeGPIODir) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);

            input.get<DeviceContainer>(0).ptr->writeGPIODir(
                input.get<std::string>(1),
                input.get<unsigned>(2));
        },
        "Device_writeGPIODir");
}

MEX_DEFINE(Device_writeGPIODirMasked) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);

            input.get<DeviceContainer>(0).ptr->writeGPIODir(
                input.get<std::string>(1),
                input.get<unsigned>(2),
                input.get<unsigned>(3));
        },
        "Device_writeGPIODirMasked");
}

//
// I2C API
//

MEX_DEFINE(Device_writeI2C) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);

            input.get<DeviceContainer>(0).ptr->writeI2C(
                input.get<int>(1),
                input.get<std::string>(2));
        },
        "Device_writeI2C");
}

MEX_DEFINE(Device_readI2C) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->readI2C(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_readI2C");
}

//
// SPI API
//

MEX_DEFINE(Device_transactSPI) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->transactSPI(
                    input.get<int>(1),
                    input.get<unsigned>(2),
                    input.get<size_t>(3)));
        },
        "Device_transactSPI");
}

//
// UART API
//

MEX_DEFINE(Device_listUARTs) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->listUARTs());
        },
        "Device_listUARTs");
}

MEX_DEFINE(Device_writeUART) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);

            input.get<DeviceContainer>(0).ptr->writeUART(
                input.get<std::string>(1),
                input.get<std::string>(2));
        },
        "Device_writeUART");
}

MEX_DEFINE(Device_readUART) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->readUART(
                    input.get<std::string>(1),
                    input.get<long>(2)));
        },
        "Device_readUART");
}

//////////////////////////////////////////////////////
// mexplus
//////////////////////////////////////////////////////

MEX_DISPATCH
