% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

classdef SoapySDR_StreamFlag
    properties (Constant)
        EndBurst      = bitshift(1, 1)
        HasTime       = bitshift(1, 2)
        EndAbrupt     = bitshift(1, 3)
        OnePacket     = bitshift(1, 4)
        MoreFragments = bitshift(1, 5)
        WaitTrigger   = bitshift(1, 6)
        UserFlag0     = bitshift(1, 16)
        UserFlag1     = bitshift(1, 17)
        UserFlag2     = bitshift(1, 18)
        UserFlag3     = bitshift(1, 19)
        UserFlag4     = bitshift(1, 20)
    end
end
