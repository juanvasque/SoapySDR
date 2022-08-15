% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

classdef SoapySDR_Stream < handle
    properties
        direction
        format
        channels
        args
        mtu
    end

    properties (Access = protected)
        __internal
    end

    methods
        %
        % Construction/destruction
        %

        function this = SoapySDR_Stream(internal)
            this.__internal = internal;

            this.direction = this.__internal.direction;
            this.format = this.__internal.format;
            this.channels = this.__internal.channels;
            this.args = this.__internal.args;
            this.mtu = this.__internal.mtu;
        end

        function delete(this)
        %DELETE Destructor.
            % TODO
        end

        %
        % Operations
        %

        function close(this)
        %CLOSE Close the stream.
            SoapySDR_MEX("Stream_close", this.__internal);
        end

        function activate(this, flags, timeNs, numElems)
        %ACTIVATE Begin streaming.
            flags_ = 0;
            timeNs_ = 0;
            numElems_ = 0;

            if nargin >= 2
                flags_ = flags;
            end
            if nargin >= 3
                timeNs_ = timeNs;
            end
            if nargin >= 4
                numElems_ = numElems;
            end

            SoapySDR_MEX("Stream_activate", this.__internal, flags_, timeNs_, numElems_);
        end

        function deactivate(this, flags, timeNs)
        %DEACTIVATE Stop streaming.
            flags_ = 0
            timeNs_ = 0

            if nargin >= 2
                flags_ = flags;
            end
            if nargin >= 3
                timeNs_ = timeNs;
            end

            SoapySDR_MEX("Stream_deactivate", this.__internal, flags_, timeNs_);
        end
end
