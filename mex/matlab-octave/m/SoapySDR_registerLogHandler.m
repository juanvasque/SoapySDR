% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

function SoapySDR_registerLogHandler(handler)
%REGISTERLOGHANDLER Register the given function as the new log handler
    SoapySDR_MEX("Logger_registerLogHandler", handler);
end
