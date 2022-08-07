% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

function output = SoapySDR_Time_timeNsToTicks(timeNs, rate)
%TIMENSTOTICKS Convert nanoseconds ticks.
    output = SoapySDR_MEX("Time_timeNsToTicks", timeNs, rate);
end
