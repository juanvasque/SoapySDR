% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

function SoapySDR_clearLogHandler()
%CLEARLOGHANDLER Restore the log handler to the default handler.
    SoapySDR_MEX("Logger_clearLogHandler");
end
