% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

function SoapySDR_logf(level, format, varargin)
%LOGF Log the given formatted message
    SoapySDR_MEX("Logger_log", level, sprintf(format, varargin{:}));
end
