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
    end

    methods (Static)
        function devices = enumerate(args)
            args_ = ""
            if nargin > 0
                assert(ischar(args))
                args_ = args
            end

            devices = SoapySDR_MEX("Device_enumerate", args_);
        end
    end
end
