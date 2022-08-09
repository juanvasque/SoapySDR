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
end
