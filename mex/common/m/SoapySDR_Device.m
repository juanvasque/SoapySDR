% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

classdef SoapySDR_Device < handle
    properties
        driverKey
        hardwareKey
        hardwareInfo
    end

    properties (Access = private)
        __internal
    end

    methods
        %
        % Construction/destruction
        %

        function this = SoapySDR_Device(args)
        %MAKE Instantiate a new device.
            args_ = "";
            if nargin > 0
                assert(ischar(args));
                args_ = args;
            end

            this.__internal = SoapySDR_MEX("Device_make", args_);
            this.driverKey = this.__internal.driverKey;
            this.hardwareKey = this.__internal.hardwareKey;
            this.hardwareInfo = this.__internal.hardwareInfo;
        end

        function delete(this)
        %DELETE Destructor.
            SoapySDR_MEX("Device_unmake", this.__internal);
        end

        %
        % Channels API
        %

        function setFrontendMapping(this, direction, mapping)
        %SETFRONTENDMAPPING Set frontend mapping.
            SoapySDR_MEX("Device_setFrontendMapping", this.__internal, direction, mapping);
        end

        function mapping = getFrontendMapping(this, direction)
        %GETFRONTENDMAPPING Get frontend mapping.
            mapping = SoapySDR_MEX("Device_getFrontendMapping", this.__internal, direction);
        end

        function numChannels = getNumChannels(this, direction)
        %GETNUMCHANNELS Get number of channels.
            numChannels = SoapySDR_MEX("Device_getNumChannels", this.__internal, direction);
        end

        function channelInfo = getChannelInfo(this, direction, channel)
        %GETCHANNELINFO Get channel info.
            channelInfo = SoapySDR_MEX("Device_getChannelInfo", this.__internal, direction, channel);
        end

        function fullDuplex = getFullDuplex(this, direction, channel)
        %GETFULLDUPLEX Is the given channel full duplex?
            fullDuplex = SoapySDR_MEX("Device_getFullDuplex", this.__internal, direction, channel);
        end

        %
        % Stream API
        %

        function streamFormats = getStreamFormats(this, direction, channel)
        %GETSTREAMFORMATS Get the supported stream formats for the given channel.
            streamFormats = SoapySDR_MEX("Device_getStreamFormats", this.__internal, direction, channel);
        end

        function output = getNativeStreamFormat(this, direction, channel)
        %GETSTREAMFORMATS Get the supported stream formats for the given channel.
            % TODO: figure out why multiple outputs aren't working
            output = SoapySDR_MEX("Device_getNativeStreamFormat", this.__internal, direction, channel);
        end

        function streamArgsInfo = getStreamArgsInfo(this, direction, channel)
        %GETSTREAMFORMATS Get the supported stream formats for the given channel.
            streamArgsInfo = SoapySDR_MEX("Device_getStreamArgsInfo", this.__internal, direction, channel);
        end

        %
        % Antenna API
        %

        function antennas = listAntennas(this, direction, channel)
        %LISTANTENNAS Get antennas for the given channel.
            antennas = SoapySDR_MEX("Device_listAntennas", this.__internal, direction, channel);
        end

        function setAntenna(this, direction, channel, antenna)
        %SETANTENNA Set the antenna for the given channel.
            SoapySDR_MEX("Device_setAntenna", this.__internal, direction, channel, antenna)
        end

        function antenna = getAntenna(this, direction, channel)
        %GETANTENNA Get the antenna for the given channel.
            antenna = SoapySDR_MEX("Device_getAntenna", this.__internal, direction, channel);
        end

        %
        % Frontend Corrections API
        %

        function value = hasDCOffsetMode(this, direction, channel)
        %HASDCOFFSETMODE Does the given channel support automatic frontend DC offset correction?
            value = SoapySDR_MEX("Device_hasDCOffsetMode", this.__internal, direction, channel);
        end

        function setDCOffsetMode(this, direction, channel, dcOffsetMode)
        %SETDCOFFSETMODE Set the frontend DC offset correction policy for the given channel.
            SoapySDR_MEX("Device_setDCOffsetMode", this.__internal, direction, channel, dcOffsetMode);
        end

        function dcOffsetMode = getDCOffsetMode(this, direction, channel)
        %GETDCOFFSETMODE Get the frontend DC offset correction policy for the given channel.
            dcOffsetMode = SoapySDR_MEX("Device_getDCOffsetMode", this.__internal, direction, channel);
        end

        function value = hasDCOffset(this, direction, channel)
        %HASDCOFFSET Does the given channel support frontend DC offset correction?
            value = SoapySDR_MEX("Device_hasDCOffset", this.__internal, direction, channel);
        end

        function setDCOffset(this, direction, channel, dcOffset)
        %SETDCOFFSET Set the frontend DC offset correction for the given channel.
            SoapySDR_MEX("Device_setDCOffset", this.__internal, direction, channel, dcOffset);
        end

        function dcOffset = getDCOffset(this, direction, channel)
        %GETDCOFFSET Get the frontend DC offset correction for the given channel.
            dcOffset = SoapySDR_MEX("Device_getDCOffset", this.__internal, direction, channel);
        end

        function value = hasIQBalanceMode(this, direction, channel)
        %HASIQBALANCEMODE Does the given channel support automatic frontend IQ balance correction?
            value = SoapySDR_MEX("Device_hasIQBalanceMode", this.__internal, direction, channel);
        end

        function setIQBalanceMode(this, direction, channel, iqBalanceMode)
        %SETIQBALANCEMODE Set the frontend IQ balance correction policy for the given channel.
            SoapySDR_MEX("Device_setIQBalanceMode", this.__internal, direction, channel, iqBalanceMode);
        end

        function iqBalanceMode = getIQBalanceMode(this, direction, channel)
        %GETIQBALANCEMODE Get the frontend IQ balance correction policy for the given channel.
            iqBalanceMode = SoapySDR_MEX("Device_getIQBalanceMode", this.__internal, direction, channel);
        end

        function value = hasIQBalance(this, direction, channel)
        %HASIQBALANCE Does the given channel support frontend IQ balance correction?
            value = SoapySDR_MEX("Device_hasIQBalance", this.__internal, direction, channel);
        end

        function setIQBalance(this, direction, channel, iqBalance)
        %SETIQBALANCE Set the frontend IQ balance correction for the given channel.
            SoapySDR_MEX("Device_setIQBalance", this.__internal, direction, channel, iqBalance);
        end

        function iqBalance = getIQBalance(this, direction, channel)
        %GETIQBALANCE Get the frontend IQ balance correction for the given channel.
            iqBalance = SoapySDR_MEX("Device_getIQBalance", this.__internal, direction, channel);
        end

        function value = hasFrequencyCorrection(this, direction, channel)
        %HASFREQUENCYCORRECTION Does the given channel support frontend frequency correction?
            value = SoapySDR_MEX("Device_hasFrequencyCorrection", this.__internal, direction, channel);
        end

        function setFrequencyCorrection(this, direction, channel, frequencyCorrection)
        %SETFREQUENCYCORRECTION Set the frontend frequency correction for the given channel.
            SoapySDR_MEX("Device_setFrequencyCorrection", this.__internal, direction, channel, frequencyCorrection);
        end

        function frequencyCorrection = getFrequencyCorrection(this, direction, channel)
        %GETFREQUENCYCORRECTION Get the frontend frequency correction for the given channel.
            frequencyCorrection = SoapySDR_MEX("Device_getFrequencyCorrection", this.__internal, direction, channel);
        end

        %
        % Gain API
        %

        function gains = listGains(this, direction, channel)
        %LISTGAINS List the valid gain elements for this channel.
            gains = SoapySDR_MEX("Device_listGains", this.__internal, direction, channel);
        end

        function value = hasGainMode(this, direction, channel)
        %HASGAINMODE Does the given channel support automatic frontend gain correction?
            value = SoapySDR_MEX("Device_hasGainMode", this.__internal, direction, channel);
        end

        function setGainMode(this, direction, channel, gainMode)
        %SETGAINMODE Set the frontend gain correction policy for the given channel.
            SoapySDR_MEX("Device_setGainMode", this.__internal, direction, channel, gainMode);
        end

        function gainMode = getGainMode(this, direction, channel)
        %GETGAINMODE Get the frontend gain correction policy for the given channel.
            gainMode = SoapySDR_MEX("Device_getGainMode", this.__internal, direction, channel);
        end

        function setGain(this, direction, channel, gain)
        %SETGAIN Set the overall gain for the given channel.
            SoapySDR_MEX("Device_setGain", this.__internal, direction, channel, gain);
        end

        function setGainElement(this, direction, channel, name, gain)
        %SETGAINELEMENT Set the gain element's value for the given channel.
            SoapySDR_MEX("Device_setGainElement", this.__internal, direction, channel, name, gain);
        end

        function gain = getGain(this, direction, channel)
        %GETGAIN Get the overall gain for the given channel.
            gain = SoapySDR_MEX("Device_getGain", this.__internal, direction, channel);
        end

        function gain = getGainElement(this, direction, channel, name)
        %GETGAINELEMENT Get the gain element's value for the given channel.
            gain = SoapySDR_MEX("Device_getGainElement", this.__internal, direction, channel, name);
        end

        function gainRange = getGainRange(this, direction, channel)
        %GETGAINRANGE Get the overall gain range for the given channel.
            gainRange = SoapySDR_MEX("Device_getGainRange", this.__internal, direction, channel);
        end

        function gainRange = getGainElementRange(this, direction, channel, name)
        %GETGAINELEMENTRANGE Get the overall gain range for the given channel.
            gainRange = SoapySDR_MEX("Device_getGainElementRange", this.__internal, direction, channel, name);
        end

        %
        % Frequency API
        %

        function setFrequency(this, direction, channel, frequency, args)
        %SETFREQUENCY Set the overall frequency for the given channel.
            args_ = ""
            if nargin > 4
                args_ = args
            end

            SoapySDR_MEX("Device_setFrequency", this.__internal, direction, channel, frequency, args_);
        end

        function setFrequencyComponent(this, direction, channel, frequency, name, args)
        %SETFREQUENCYCOMPONENT Set a frequency component's value for the given channel.
            args_ = ""
            if nargin > 4
                args_ = args
            end

            SoapySDR_MEX("Device_setFrequencyComponent", this.__internal, direction, channel, frequency, name, args_);
        end

        function frequency = getFrequency(this, direction, channel)
        %GETFREQUENCY Get the overall frequency for the given channel.
            frequency = SoapySDR_MEX("Device_getFrequency", this.__internal, direction, channel);
        end

        function frequency = getFrequencyComponent(this, direction, channel, name)
        %GETFREQUENCYCOMPONENT Get the value of a frequency component for the given channel.
            frequency = SoapySDR_MEX("Device_getFrequencyComponent", this.__internal, direction, channel, name);
        end

        function frequencyRange = getFrequencyRange(this, direction, channel)
        %GETFREQUENCY Get the overall frequency rangefor the given channel.
            frequency = SoapySDR_MEX("Device_getFrequencyRange", this.__internal, direction, channel);
        end

        function frequencyRange = getFrequencyComponentRange(this, direction, channel, name)
        %GETFREQUENCYCOMPONENT Get the range of a frequency component for the given channel.
            frequencyRange = SoapySDR_MEX("Device_getFrequencyComponentRange", this.__internal, direction, channel, name);
        end

        %
        % Sample Rate API
        %

        function setSampleRate(this, direction, channel, rate)
        %SETSAMPLERATE Set the sample rate for the given channel.
            SoapySDR_MEX("Device_setSampleRate", this.__internal, direction, channel, rate);
        end

        function sampleRate = getSampleRate(this, direction, channel)
        %GETSAMPLERATE Get the sample rate for the given channel.
            sampleRate = SoapySDR_MEX("Device_getSampleRate", this.__internal, direction, channel);
        end

        function sampleRateRange = getSampleRateRange(this, direction, channel)
        %GETSAMPLERATERANGE Get the sample rate range for the given channel.
            sampleRateRange = SoapySDR_MEX("Device_getSampleRateRange", this.__internal, direction, channel);
        end

        %
        % Bandwidth API
        %

        function setBandwidth(this, direction, channel, rate)
        %SETBANDWIDTH Set the bandwidth for the given channel.
            SoapySDR_MEX("Device_setBandwidth", this.__internal, direction, channel, rate);
        end

        function bandwidth = getBandwidth(this, direction, channel)
        %GETBANDWIDTH Get the bandwidth for the given channel.
            bandwidth = SoapySDR_MEX("Device_getBandwidth", this.__internal, direction, channel);
        end

        function bandwidthRange = getBandwidthRange(this, direction, channel)
        %GETBANDWIDTHRANGE Get the bandwidth range for the given channel.
            bandwidthRange = SoapySDR_MEX("Device_getBandwidthRange", this.__internal, direction, channel);
        end

        %
        % Clocking API
        %

        function setMasterClockRate(this, rate)
        %SETMASTERCLOCKRATE Set the master clock rate.
            SoapySDR_MEX("Device_setMasterClockRate", this.__internal, rate);
        end

        function masterClockRate = getMasterClockRate(this)
        %GETMASTERCLOCKRATE Get the master clock rate.
            masterClockRate = SoapySDR_MEX("Device_getMasterClockRate", this.__internal);
        end

        function masterClockRates = getMasterClockRates(this)
        %GETMASTERCLOCKRATES Get the valid master clock rates.
            masterClockRates = SoapySDR_MEX("Device_getMasterClockRates", this.__internal);
        end

        function setReferenceClockRate(this, rate)
        %SETREFERENCECLOCKRATE Set the reference clock rate.
            SoapySDR_MEX("Device_setReferenceClockRate", this.__internal, rate);
        end

        function referenceClockRate = getReferenceClockRate(this)
        %GETREFERENCECLOCKRATE Get the reference clock rate.
            referenceClockRate = SoapySDR_MEX("Device_getReferenceClockRate", this.__internal);
        end

        function referenceClockRates = getReferenceClockRates(this)
        %GETREFERENCECLOCKRATES Get the valid reference clock rates.
            referenceClockRates = SoapySDR_MEX("Device_getReferenceClockRates", this.__internal);
        end

        function setClockSource(this, rate)
        %SETCLOCKSOURCE Set the clock source.
            SoapySDR_MEX("Device_setClockSource", this.__internal, rate);
        end

        function clockSource = getClockSource(this)
        %GETCLOCKSOURCE Get the clock source.
            clockSource = SoapySDR_MEX("Device_getClockSource", this.__internal);
        end

        function clockSources = listClockSources(this)
        %GETCLOCKSOURCES list the valid clock sources.
            clockSources = SoapySDR_MEX("Device_listClockSources", this.__internal);
        end

        %
        % Time API
        %

        function setTimeSource(this, rate)
        %SETTIMESOURCE Set the time source.
            SoapySDR_MEX("Device_setTimeSource", this.__internal, rate);
        end

        function clockSource = getClockSource(this)
        %GETCLOCKSOURCE Get the clock source.
            clockSource = SoapySDR_MEX("Device_getClockSource", this.__internal);
        end

        function clockSources = listClockSources(this)
        %GETCLOCKSOURCES list the valid clock sources.
            clockSources = SoapySDR_MEX("Device_listClockSources", this.__internal);
        end

        function value = hasHardwareTime(this, what)
        %HASHARDWARETIME Does this device have a hardware clock?
            what_ = ""
            if nargin > 1
                what_ = what
            end

            value = SoapySDR_MEX("Device_hasHardwareTime", this, what_)
        end

        function value = getHardwareTime(this, what)
        %GETHARDWARETIME Read the time from the device's hardware clock.
            what_ = ""
            if nargin > 1
                what_ = what
            end

            value = SoapySDR_MEX("Device_getHardwareTime", this, what_)
        end

        function setHardwareTime(this, timeNs, what)
        %SETHARDWARETIME Write the time to the device's hardware clock.
            what_ = ""
            if nargin > 2
                what_ = what
            end

            SoapySDR_MEX("Device_setHardwareTime", this, timeNs, what_)
        end
    end

    %
    % Enumeration
    %

    methods (Static)
        function devices = enumerate(args)
            args_ = "";
            if nargin > 0
                assert(ischar(args));
                args_ = args;
            end

            devices = SoapySDR_MEX("Device_enumerate", args_);
        end
    end
end
