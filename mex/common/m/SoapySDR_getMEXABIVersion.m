% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

function output = SoapySDR_getMEXABIVersion()
%GETMEXABIVERSION Get the SoapySDR ABI version the MEX file was built against.
    output = SoapySDR_MEX("Version_getMEXABIVersion");
end
