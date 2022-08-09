% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

classdef SoapySDR_TxStream < SoapySDR_Stream
    methods
        %
        % Construction/destruction
        %

        function this = SoapySDR_TxStream(internal)
            s@SoapySDR_Stream(internal);
        end

        %
        % Methods
        %

        function result = write(this, samples, timeNs, timeoutUs)
        %WRITE Transmit the given samples.
            timeNs_ = 0;
            timeoutUs_ = 100000;

            if nargin >= 3
                timeNs_ = timeNs;
            end
            if nargin >= 4
                timeoutUs_ = timeoutUs;
            end

            % TODO: distinguish input types, call applicable MEX function
        end

        function status = readStatus(this, timeoutUs)
        %READSTATUS Async read the current stream status.
            timeoutUs_ = 100000;

            if nargin >= 2
                timeoutUs_ = timeoutUs;
            end

            status = SoapySDR_MEX("Stream_readStatus", this.__internal, timeoutUs_);
        end
end
