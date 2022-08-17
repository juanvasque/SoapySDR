% Copyright (c) 2022 Nicholas Corgan
% SPDX-License-Identifier: BSL-1.0

%
% Parameters
%

args = "driver=spyserver,host=125.160.16.157,port=5555";


%args = "";
chan = 0;
direction = SoapySDR_Direction.Rx;
format = SoapySDR_StreamFormat.CF32;
frequency = 100e6;
gain = 0;
rate = 912e3;
nsamps = 10e6;

%
% Initialize device.
%

device = SoapySDR_Device(args);

%
% Set device settings.

%

device.setFrequency(direction, chan, frequency);
printf("Device frequency set to %f MHz.\n", device.getFrequency(direction, chan)/1e6);

device.setGain(direction, chan, gain);
printf("Device gain set to %f dB.\n", device.getGain(direction, chan));

device.setSampleRate(direction, chan, rate);
printf("Device sample rate set to %f MHz.\n", device.getSampleRate(direction, chan)/1e6);

%
% Initialize stream and read samples.
%

stream = device.setupStream(direction, format, [chan]);
stream.activate();

printf("Reading %d samples...\n", nsamps);

sampsRemaining = nsamps;
while sampsRemaining > 0
    sampsToRead = min(sampsRemaining, stream.mtu);
    result = stream.read(sampsToRead);
    if result.errorCode != 0
        printf("Stream error: %s\n", SoapySDR_ErrorCode.toString(result.errorCode));
        break
    end

    numel(result.samples)
    sampsRemaining -= sampsToRead;
end

% Deactivate and close the stream.
stream.deactivate();
stream.close();

% Display our plots.
