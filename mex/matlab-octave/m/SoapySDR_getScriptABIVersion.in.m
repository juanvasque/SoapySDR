% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

function output = SoapySDR_getScriptABIVersion()
%GETSCRIPTABIVERSION Get the ABI version of the SoapySDR library this API was built against.
    output = "@SOAPY_SDR_ABI_VERSION@";
end
