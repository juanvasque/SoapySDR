% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

function output = SoapySDR_Time_ticksToTimeNs(ticks, rate)
%TICKSTOTIMENS Convert ticks to nanoseconds.
    output = SoapySDR_MEX("Time_ticksToTimeNs", ticks, rate);
end
