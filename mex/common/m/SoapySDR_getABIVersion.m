% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

function output = SoapySDR_getABIVersion()
%GETABIVERSION Get the ABI version for the currently installed SoapySDR library.
    output = SoapySDR_MEX("Version_getABIVersion");
end
