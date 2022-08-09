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

            if this.format == SoapySDR_StreamFormat.CF32
                result = SoapySDR_MEX("Stream_readStreamCF32", this.__internal, numElems, timeoutUs_);
            elseif this.format == SoapySDR_StreamFormat.CF64
                result = SoapySDR_MEX("Stream_readStreamCF64", this.__internal, numElems, timeoutUs_);
            else
                error(sprintf("Invalid format %s", this.format));
            end
        end
end
