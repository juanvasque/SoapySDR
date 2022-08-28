% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

classdef SoapySDR_ErrorCode
    properties (Constant)
        Timeout      = -1
        StreamError  = -2
        Corruption   = -3
        Overflow     = -4,
        NotSupported = -5,
        TimeError    = -6,
        Underflow    = -7
    end

    methods (Static)
        function output = toString(errorCode)
            output = SoapySDR_MEX("Error_errToStr", errorCode);
        end
    end
end
