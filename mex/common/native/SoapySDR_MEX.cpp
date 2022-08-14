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
#include <stdexcept>
#include <vector>

using namespace mexplus;

// TODO: policy on when to bail on streaming error codes or give ability for user to bail
// TODO: explicitly fill out mandatory, do optional arguments

//////////////////////////////////////////////////////
// Utility
//////////////////////////////////////////////////////

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

// https://stackoverflow.com/a/4541470
static std::string demangle(const char *name)
{
    int status = 0;

    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };

    return (status==0) ? res.get() : name;
}

#else

static inline std::string demangle(const char *name)
{
    return name;
}

#endif

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
    std::string ret;

#define CASE(x) \
    case SoapySDR::ArgInfo::x: \
        ret = #x; \
        break;

    // TODO: as string
    switch(type)
    {
    CASE(BOOL)
    CASE(INT)
    CASE(FLOAT)
    CASE(STRING)

    default:
        ret = "INVALID";
        break;
    }

    return MxArray::from(ret);
#undef CASE
}

template <>
inline mxArray *MxArray::from(const SoapySDRArgInfoType &type)
{
    return MxArray::from(int(type));
}

template <>
inline mxArray *MxArray::from(const SoapySDRLogLevel &level)
{
    return MxArray::from(int(level));
}

template <>
void MxArray::to(const mxArray *array, SoapySDRLogLevel *level)
{
    if(!level)
        throw std::runtime_error("MxArray::to<SoapySDRLogLevel>: null pointer exception. This is an internal bug and should be reported");

    int intVal;
    MxArray::to(array, &intVal);

    *level = SoapySDRLogLevel(intVal);
}

// For simplicity, use strings in this layer
template <>
mxArray *MxArray::from(const SoapySDR::Kwargs &args)
{
    std::string output("{");
    output += SoapySDR::KwargsToString(args);
    output += "}";

    return MxArray::from(output);
}

template <typename Fcn>
static void safeCall(const Fcn &fcn, const std::string &context)
{
    try { fcn(); }
    catch(const std::exception &ex)
    {
        std::string errorMsg(context);
        errorMsg += ": caught ";
        errorMsg += demangle(typeid(ex).name());
        errorMsg += " (";
        errorMsg += ex.what();
        errorMsg += ")";

        mexErrMsgTxt(errorMsg.c_str());
    }
    catch(...)
    {
        std::string errorMsg(context);
        errorMsg += ": caught unknown error";

        mexErrMsgTxt(errorMsg.c_str());
    }
}

template <typename Fcn>
static void safeCallWithErrorCode(const Fcn &fcn, const std::string &context)
{
    try
    {
        int errorCode = fcn();
        if(errorCode)
            mexErrMsgIdAndTxt(context.c_str(), SoapySDR::errToStr(errorCode));
    }
    catch(const std::exception &ex)
    {
        std::string errorMsg(context);
        errorMsg += ": caught ";
        errorMsg += demangle(typeid(ex).name());
        errorMsg += " (";
        errorMsg += ex.what();
        errorMsg += ")";

        mexErrMsgTxt(errorMsg.c_str());
    }
    catch(...)
    {
        std::string errorMsg(context);
        errorMsg += ": caught unknown error";

        mexErrMsgTxt(errorMsg.c_str());
    }
}

//////////////////////////////////////////////////////
// <SoapySDR/Errors.hpp>
//////////////////////////////////////////////////////

MEX_DEFINE(Error_errToStr) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                SoapySDR::errToStr(input.get<int>(0)));
        },
        "ticksToTimeNs");
}

//////////////////////////////////////////////////////
// <SoapySDR/Logger.hpp>
//////////////////////////////////////////////////////

static mxArray *mxLoggerFcn = nullptr;

static void SoapyLogHandler(const SoapySDRLogLevel logLevel, const char *message)
{
    safeCall(
        [&]()
        {
            if(!mxLoggerFcn)
                throw std::runtime_error("SoapyLogHandler called without an active log handler. This is an internal bug and should be reported.");
            if(!message)
                throw std::runtime_error("SoapyLogHandler: null message. This is an internal bug and should be reported.");

            mxArray *lhs{nullptr};
            mxArray *rhs[3]
            {
                mxLoggerFcn,
                MxArray::from(logLevel),
                MxArray::from(message),
            };

            mexCallMATLAB(0, &lhs, ARRAY_SIZE(rhs), rhs, "feval");

            // Clean up
            mxDestroyArray(rhs[2]);
            mxDestroyArray(rhs[1]);
        },
        "SoapyLogHandler");
}

MEX_DEFINE(Logger_log) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);

            SoapySDR::log(
                input.get<SoapySDRLogLevel>(0),
                input.get<std::string>(1).c_str());
        },
        "Logger_log");
}

MEX_DEFINE(Logger_setLogLevel) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);

            SoapySDR::setLogLevel(input.get<SoapySDRLogLevel>(0));
        },
        "Logger_setLogLevel");
}

MEX_DEFINE(Logger_registerLogHandler) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            if(nrhs != 1)
                throw std::invalid_argument("Logger_registerLogHandler: expected one input argument.");
            if(!prhs[0])
                throw std::invalid_argument("Logger_registerLogHandler: null argument.");
            if(!mxIsClass(prhs[0], "function_handle"))
                throw std::invalid_argument("Logger_registerLogHandler: expected a function handle.");

            // Note: as it is now, the last one set will leak, but that's better
            // than crashing.
            mxLoggerFcn = mxDuplicateArray(prhs[0]);
            mexMakeArrayPersistent(mxLoggerFcn);

            SoapySDR::registerLogHandler(&SoapyLogHandler);
        },
        "Logger_registerLogHandler");
}

MEX_DEFINE(Logger_clearLogHandler) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            mxLoggerFcn = nullptr;
            SoapySDR::registerLogHandler(nullptr);
        },
        "Logger_clearLogHandler");
}

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

// Since we can't specialize MxArray::from() for pointers
template <typename T>
T * getPointerField(const mxArray *array, const std::string &name)
{
    if(!array)
        throw std::runtime_error("getPointerField: null pointer exception. This is an internal bug and should be reported");

    uintptr_t uintptr = 0;
    MxArray::at(array, name.c_str(), &uintptr);

    return reinterpret_cast<T *>(uintptr);
}

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

    if(device.ptr)
    {
        struct_array.set("driverKey", device.ptr->getDriverKey());
        struct_array.set("hardwareKey", device.ptr->getHardwareKey());
        struct_array.set("hardwareInfo", device.ptr->getHardwareInfo());
        struct_array.set("__internal", reinterpret_cast<uintptr_t>(device.ptr));
    }

    return struct_array.release();
}

template <>
void MxArray::to(const mxArray *array, DeviceContainer *device)
{
    if(!device)
        throw std::runtime_error("MxArray::to<DeviceContainer>: null pointer exception. This is an internal bug and should be reported");

    device->ptr = getPointerField<SoapySDR::Device>(array, "__internal");
}

//
// Stream helper structs
//

struct StreamContainer
{
    SoapySDR::Stream *stream{nullptr};
    SoapySDR::Device *device{nullptr};

    int direction{SOAPY_SDR_TX};
    std::string format;
    std::vector<size_t> channels;
    std::string args;
    size_t mtu{0};
};

template <typename T>
struct RxStreamResult
{
    int errorCode{0};

    std::vector<std::vector<std::complex<T>>> samples;
    int flags{0};
    long long timeNs{0};
};

struct TxStreamResult
{
    int errorCode{0};

    size_t elemsWritten{0};
    int flags{0};
};

struct StreamStatus
{
    int errorCode{0};

    size_t chanMask{0};
    int flags{0};
    long long timeNs{0};
};

template <>
mxArray *MxArray::from(const StreamContainer &stream)
{
    const char *fields[] = {"format", "channels", "args", "mtu", "__internalStream", "__internalDevice"};
    MxArray struct_array(MxArray::Struct(ARRAY_SIZE(fields), fields));

    struct_array.set("direction", stream.direction);
    struct_array.set("format", stream.format);
    struct_array.set("channels", stream.channels);
    struct_array.set("args", stream.args);
    struct_array.set("mtu", stream.device->getStreamMTU(stream.stream));
    struct_array.set("__internalStream", reinterpret_cast<uintptr_t>(stream.stream));
    struct_array.set("__internalDevice", reinterpret_cast<uintptr_t>(stream.device));

    return struct_array.release();
}

template <>
void MxArray::to(const mxArray *array, StreamContainer *stream)
{
    if(!stream)
        throw std::runtime_error("MxArray::to<StreamContainer>: null pointer exception. This is an internal bug and should be reported");

    MxArray::at(array, "format", &stream->format);
    MxArray::at(array, "channels", &stream->channels);
    MxArray::at(array, "args", &stream->args);
    MxArray::at(array, "mtu", &stream->mtu);
    stream->stream = getPointerField<SoapySDR::Stream>(array, "__internalStream");
    stream->device = getPointerField<SoapySDR::Device>(array, "__internalDevice");
}

// We need this because we can't do partially specialized templates.
// TODO: does this end up with the matrices we want?
#define MXARRAY_FROM_RXSTREAMRESULT(T) \
    template <> \
    mxArray *MxArray::from(const RxStreamResult<T> &result) \
    { \
        const char *fields[] = {"errorCode", "samples", "flags", "timeNs"}; \
        MxArray struct_array(MxArray::Struct(ARRAY_SIZE(fields), fields)); \
 \
        struct_array.set("errorCode", result.errorCode); \
        struct_array.set("samples", result.samples); \
 \
        return struct_array.release(); \
    }

MXARRAY_FROM_RXSTREAMRESULT(float)
MXARRAY_FROM_RXSTREAMRESULT(double)

template <>
mxArray *MxArray::from(const TxStreamResult &result)
{
    const char *fields[] = {"errorCode", "elemsWritten", "flags"};
    MxArray struct_array(MxArray::Struct(ARRAY_SIZE(fields), fields));

    struct_array.set("errorCode", result.errorCode);
    struct_array.set("elemsWritten", result.elemsWritten);
    struct_array.set("flags", result.flags);

    return struct_array.release();
}

template <>
mxArray *MxArray::from(const StreamStatus &status)
{
    const char *fields[] = {"errorCode", "chanMask", "flags", "timeNs"};
    MxArray struct_array(MxArray::Struct(ARRAY_SIZE(fields), fields));

    struct_array.set("errorCode", status.errorCode);
    struct_array.set("chanMask", status.chanMask);
    struct_array.set("flags", status.flags);
    struct_array.set("timeNs", status.timeNs);

    return struct_array.release();
}

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

            // Make sure the ABI matches before doing anything.
            static const std::string buildTimeABI(SOAPY_SDR_ABI_VERSION);
            if(SoapySDR::getABIVersion() != buildTimeABI)
            {
                std::string errorMsg("Failed ABI check. SoapySDR ");
                errorMsg += SoapySDR::getABIVersion();
                errorMsg += ", MEX bindings ";
                errorMsg += buildTimeABI;
                errorMsg += ". Rebuild the module.";

                throw std::runtime_error(errorMsg);
            }

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

MEX_DEFINE(Device_setupStream) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 5);
            OutputArguments output(nlhs, plhs, 1);

            const auto deviceContainer = input.get<DeviceContainer>(0);
            const auto format = input.get<std::string>(4);

            if((format != SOAPY_SDR_CF32) and (format != SOAPY_SDR_CF64))
                throw std::invalid_argument("Matlab/Octave bindings only support formats CF32 and CF64.");

            StreamContainer streamContainer
            {
                nullptr,
                deviceContainer.ptr,
                input.get<int>(1),
                input.get<std::string>(2),
                input.get<std::vector<size_t>>(3),
                format
            };
            streamContainer.stream = streamContainer.device->setupStream(
                streamContainer.direction,
                streamContainer.format,
                streamContainer.channels,
                SoapySDR::KwargsFromString(streamContainer.args));

            if(!streamContainer.stream)
                throw std::runtime_error("Failed to initialize stream");

            output.set(0, streamContainer);
        },
        "Device_setupStream");
}

MEX_DEFINE(Stream_close) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 1);

            auto stream = input.get<StreamContainer>(0);
            stream.device->closeStream(stream.stream);
        },
        "Stream_close");
}

MEX_DEFINE(Stream_activate) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCallWithErrorCode(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);

            auto stream = input.get<StreamContainer>(0);
            return stream.device->activateStream(
                stream.stream,
                input.get<int>(1),
                input.get<long long>(2),
                input.get<size_t>(3));
        },
        "Stream_activate");
}

MEX_DEFINE(Stream_deactivate) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCallWithErrorCode(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);

            auto stream = input.get<StreamContainer>(0);
            return stream.device->deactivateStream(
                stream.stream,
                input.get<int>(1),
                input.get<long long>(2));
        },
        "Stream_deactivate");
}

template <typename T>
static void streamReadStream(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[], const std::string &expectedFormat)
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            const auto stream = input.get<StreamContainer>(0);
            if(stream.direction != SOAPY_SDR_RX)
                throw std::invalid_argument("Cannot receive with TX stream");

            if(stream.format != expectedFormat)
            {
                std::string errorMsg("Invalid format "+stream.format+". Expected "+expectedFormat+".");
                throw std::invalid_argument(errorMsg.c_str());
            }

            const auto numElems = input.get<size_t>(1);

            RxStreamResult<T> result;
            result.samples.resize(stream.channels.size());
            for(auto &chanSamps: result.samples)
                chanSamps.resize(numElems);

            std::vector<void *> buffs;
            std::transform(
                result.samples.begin(),
                result.samples.end(),
                std::back_inserter(buffs),
                [](std::vector<std::complex<T>> &vec)
                {
                    return (void*)vec.data();
                });

            const int readRet = stream.device->readStream(
                stream.stream,
                buffs.data(),
                numElems,
                result.flags,
                result.timeNs,
                input.get<long>(2));
            if(readRet > 0)
            {
                for(auto &chanSamps: result.samples)
                    chanSamps.resize(size_t(readRet));
            }
            else
            {
                for(auto &chanSamps: result.samples)
                    chanSamps.resize(0);

                result.errorCode = readRet;
            }

            output.set(0, result);
        },
        "Stream_readStream");
}

template <typename T>
static void streamWriteStream(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[], const std::string &expectedFormat)
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 4);
            OutputArguments output(nlhs, plhs, 1);

            const auto stream = input.get<StreamContainer>(0);
            if(stream.direction != SOAPY_SDR_TX)
                throw std::invalid_argument("Cannot receive with TX stream");

            if(stream.format != expectedFormat)
            {
                std::string errorMsg("Invalid format "+stream.format+". Expected "+expectedFormat+".");
                throw std::invalid_argument(errorMsg);
            }

            const auto samples = input.get<std::vector<std::vector<std::complex<T>>>>(1);
            if(samples.size() != stream.channels.size())
            {
                std::string errorMsg("Invalid sample dimensions ("+std::to_string(samples.size())+" channels). Expected "+std::to_string(stream.channels.size())+".");
                throw std::invalid_argument(errorMsg);
            }

            // TODO: do we need to check for jagged arrays? Or does input type guarantee this?
            const auto numElems = samples[0].size();

            TxStreamResult result;

            std::vector<const void *> buffs;
            std::transform(
                samples.begin(),
                samples.end(),
                std::back_inserter(buffs),
                [](const std::vector<std::complex<T>> &vec)
                {
                    return (const void*)vec.data();
                });

            const int writeRet = stream.device->writeStream(
                stream.stream,
                buffs.data(),
                numElems,
                result.flags,
                input.get<long long>(2),
                input.get<long>(3));
            if(writeRet > 0)
                result.elemsWritten = size_t(writeRet);
            else
                result.errorCode = writeRet;

            output.set(0, result);
        },
        "Stream_writeStream");
}

#define MEX_READWRITE_STREAM_API(ctype, format) \
    MEX_DEFINE(Stream_readStream ## format) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) \
    { \
        streamReadStream<ctype>(nlhs, plhs, nrhs, prhs, SOAPY_SDR_ ## format); \
    } \
    MEX_DEFINE(Stream_writeStream ## format) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) \
    { \
        streamWriteStream<ctype>(nlhs, plhs, nrhs, prhs, SOAPY_SDR_ ## format); \
    }

MEX_READWRITE_STREAM_API(float, CF32)
MEX_READWRITE_STREAM_API(double, CF64)

MEX_DEFINE(Stream_readStatus) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);
            OutputArguments output(nlhs, plhs, 1);

            const auto stream = input.get<StreamContainer>(0);

            StreamStatus status;
            status.errorCode = stream.device->readStreamStatus(
                stream.stream,
                status.chanMask,
                status.flags,
                status.timeNs,
                input.get<long>(1));

            output.set(0, status);
        },
        "Stream_readStatus");
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
// Frontend Corrections API
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
            InputArguments input(nrhs, prhs, 6);

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

MEX_DEFINE(Device_listFrequencies) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 3);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->listFrequencies(
                    input.get<int>(1),
                    input.get<size_t>(2)));
        },
        "Device_listFrequencies");
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

MEX_DEFINE(Device_readGPIODir) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    safeCall(
        [&]()
        {
            InputArguments input(nrhs, prhs, 2);
            OutputArguments output(nlhs, plhs, 1);

            output.set(
                0,
                input.get<DeviceContainer>(0).ptr->readGPIODir(input.get<std::string>(1)));
        },
        "Device_readGPIO");
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
