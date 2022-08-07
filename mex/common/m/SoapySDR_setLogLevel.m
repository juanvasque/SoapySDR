% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

function SoapySDR_setLogLevel(level)
%SETLOGLEVEL Set the new log threshold level.
    SoapySDR_MEX("Logger_setLogLevel", level);
end
