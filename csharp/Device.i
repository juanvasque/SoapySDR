// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%csmethodmodifiers SoapySDR::CSharp::Device::__ToString "private";
%csmethodmodifiers SoapySDR::CSharp::Device::__Equals "private";
%csmethodmodifiers SoapySDR::CSharp::Device::__ReadStream "private unsafe";
%csmethodmodifiers SoapySDR::CSharp::Device::__WriteStream "private unsafe";

%include <typemaps.i>

%apply double& OUTPUT { double& fullScaleOut };

// TODO: default args where appropriate
%typemap(cscode) SoapySDR::CSharp::Device %{
    public override string ToString()
    {
        return __ToString();
    }

    public override bool Equals(object other)
    {
        var otherAsDevice = other as Device;
        if(otherAsDevice) return __Equals(otherAsDevice);
        else throw new ArgumentException("Not a Device");
    }

    public override int GetHashCode()
    {
        return (GetClass().GetHashCode() ^ __ToString().GetHashCode());
    }

    public StreamHandle SetupStream<T>(Direction direction, string format, SizeList channels, Kwargs kwargs) where T: unmanaged
    {
        return SetupStream(direction, Utility.GetFormatString<T>(), channels, kwargs);
    }

    public unsafe StreamResult ReadStream<T>(StreamHandle streamHandle, ref T[] buff, long timeNs, int timeoutUs) where T: unmanaged
    {
        T[][] buffs2D = new T[][1];
        buffs2D[0] = buff;

        return ReadStream(streamHandle, buffs2D, timeNs, timeoutUs);
    }

    public unsafe StreamResult ReadStream<T>(StreamHandle streamHandle, ref T[][] buffs, long timeNs, int timeoutUs) where T: unmanaged
    {
        Utility.ValidateBuffs(streamHandle, buffs);

        System.Runtime.InteropServices.GCHandle[] handles = null;
        SizeList buffsAsSizes = null;

        Utility.ManagedArraysToSizeList(
            buffs,
            handles,
            buffsAsSizes);

        return __ReadStream(streamHandle, buffsAsSizes, (uint)buffs.Length, timeNs, timeoutUs);
    }

    public unsafe StreamResult ReadStream(StreamHandle streamHandle, IntPtr buff, uint numElems, long timeNs, int timeoutUs)
    {
        return ReadStream(streamHandle, new IntPtr{buff}, numElems, timeNs, timeoutUs);
    }

    public unsafe StreamResult ReadStream(StreamHandle streamHandle, IntPtr[] buffs, uint numElems, long timeNs, int timeoutUs)
    {
        var buffsAsSizes = new SizeList();
        foreach(var buff in buffs) buffsAsSizes.Add((UIntPtr)((void*)buff));

        return __ReadStream(streamHandle, buffsAsSizes, numElems, timeNs, timeoutUs);
    }

    public unsafe StreamResult ReadStream(StreamHandle streamHandle, UIntPtr buff, uint numElems, long timeNs, int timeoutUs)
    {
        return ReadStream(streamHandle, new UIntPtr{buff}, numElems, timeNs, timeoutUs);
    }

    public unsafe StreamResult ReadStream(StreamHandle streamHandle, UIntPtr[] buffs, uint numElems, long timeNs, int timeoutUs)
    {
        var buffsAsSizes = new SizeList();
        foreach(var buff in buffs) buffsAsSizes.Add((uint)buff);

        return __ReadStream(streamHandle, buffsAsSizes, numElems, timeNs, timeoutUs);
    }

    public unsafe StreamResult WriteStream<T>(StreamHandle streamHandle, T[] buff, long timeNs, int timeoutUs) where T: unmanaged
    {
        T[][] buffs2D = new T[][1];
        buffs2D[0] = buff;

        return WriteStream(streamHandle, buffs2D, timeNs, timeoutUs);
    }

    public unsafe StreamResult WriteStream<T>(StreamHandle streamHandle, T[][] buffs, uint numElems, long timeNs, int timeoutUs) where T: unmanaged
    {
        Utility.ValidateBuffs(streamHandle, buffs);

        System.Runtime.InteropServices.GCHandle[] handles = null;
        SizeList buffsAsSizes = null;

        Utility.ManagedArraysToSizeList(
            buffs,
            handles,
            buffsAsSizes);

        return __WriteStream(streamHandle, buffsAsSizes, (uint)buffs.Length, timeNs, timeoutUs);
    }

    public unsafe StreamResult WriteStream(StreamHandle streamHandle, IntPtr buff, uint numElems, long timeNs, int timeoutUs)
    {
        return WriteStream(streamHandle, new IntPtr{buff}, numElems, timeNs, timeoutUs);
    }

    public unsafe StreamResult WriteStream(StreamHandle streamHandle, IntPtr[] buffs, uint numElems, long timeNs, int timeoutUs)
    {
        var buffsAsSizes = new SizeList();
        foreach(var buff in buffs) buffsAsSizes.Add((UIntPtr)((void*)buff));

        return __WriteStream(streamHandle, buffsAsSizes, numElems, timeNs, timeoutUs);
    }
%}

%ignore SoapySDR::CSharp::DeviceDeleter;
%nodefaultctor SoapySDR::CSharp::Device;

%{
#include "DeviceWrapper.hpp"
%}

%include "DeviceWrapper.hpp"

%template(DeviceList) std::vector<SoapySDR::CSharp::Device>;
