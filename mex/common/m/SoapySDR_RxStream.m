% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

classdef SoapySDR_RxStream < SoapySDR_Stream
    methods
        %
        % Construction/destruction
        %

        function this = SoapySDR_RxStream(internal)
            s@SoapySDR_Stream(internal);
        end

        %
        % Methods
        %

        function result = read(this, numElems, timeoutUs)
        %READ Receive the given number of samples.
            timeoutUs_ = 100000;

            if nargin >= 3
                timeoutUs_ = timeoutUs;
            end

            result = SoapySDR_MEX("Stream_readStream", this.__internal, numElems, timeoutUs_);
        end
end
