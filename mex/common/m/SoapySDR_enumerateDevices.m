% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

function output = SoapySDR_enumerateDevices(args)
%ENUMERATE Enumerate devices
    output = SoapySDR_MEX("Device_enumerate", args);
end
