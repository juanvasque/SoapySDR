% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

function output = SoapySDR_getLibVersion()
%GETLIBVERSION Get the currently installed SoapySDR library's version.
    output = SoapySDR_MEX("Version_getLibVersion");
end
