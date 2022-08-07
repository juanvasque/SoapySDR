% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

function SoapySDR_log(level, message)
%LOG Log the given message
    SoapySDR_MEX("Logger_log", level, message);
end
