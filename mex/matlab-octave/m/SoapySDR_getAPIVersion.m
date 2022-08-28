% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

function output = SoapySDR_getAPIVersion()
%GETAPIVERSION Get the API version for the currently installed SoapySDR library.
    output = SoapySDR_MEX("Version_getAPIVersion");
end
