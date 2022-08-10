# Copyright (c) 2022 Nicholas Corgan
# SPDX-License-Identifier: BSL-1.0

function test_suite=TestDeviceAPI
    try % assignment of 'localfunctions' is necessary in Matlab >= 2016
        test_functions=localfunctions();
    catch % no problem; early Matlab versions can use initTestSuite fine
    end
    initTestSuite;

function fcn = testDirection(device, direction)
    SoapySDR;

    #
    # Channels API
    #

    device.setFrontendMapping(direction, "0:0")
    assertEqual("", device.getFrontendMapping(direction))
    device.getChannelInfo(direction, 0)
    assertTrue(device.getFullDuplex(direction, 0))

    #
    # Stream API
    #

    assertEqual(0, length(device.getStreamFormats(direction, 0)))
    [format, fullScale] = device.getNativeStreamFormat(direction, 0)
    assertEqual(SoapySDR.StreamFormat.CS16, format)
    assert(fullScale == bitshift(1, 15))
    assertEqual(0, length(device.getStreamArgsInfo(direction, 0)))

    #
    # Antenna API
    #

    assertEqual(0, length(device.listAntennas(direction, 0)))
    assertEqual("", device.getAntenna(direction, 0))
    device.setAntenna(direction, 0, "ANT")

    #
    # Frontend corrections API
    #

    assertFalse(device.hasDCOffsetMode(direction, 0))
    assertFalse(device.getDCOffsetMode(direction, 0))
    device.setDCOffsetMode(direction, 0, true)

    assertFalse(device.hasDCOffset(direction, 0))
    assertTrue(isnumeric(device.getDCOffset(direction, 0))) # We can't check for complex, it gets reduced to scalar
    device.setDCOffset(direction, 0, 1)
    device.setDCOffset(direction, 0, 1i)
    device.setDCOffset(direction, 0, 1 + 1i)

    assertFalse(device.hasIQBalanceMode(direction, 0))
    assertFalse(device.getIQBalanceMode(direction, 0))
    device.setIQBalanceMode(direction, 0, true)

    assertFalse(device.hasIQBalance(direction, 0))
    assertTrue(isnumeric(device.getIQBalance(direction, 0))) # We can't check for complex, it gets reduced to scalar
    device.setIQBalance(direction, 0, 1)
    device.setIQBalance(direction, 0, 1i)
    device.setIQBalance(direction, 0, 1 + 1i)

    assertFalse(device.hasFrequencyCorrection(direction, 0))
    assertEqual(0.0, device.getFrequencyCorrection(direction, 0))
    device.setFrequencyCorrection(direction, 0, 0.0)

    #
    # Gain API
    #

    assertEqual(0, length(device.listGains(direction, 0)))

    assertFalse(device.hasGainMode(direction, 0))
    assertFalse(device.getGainMode(direction, 0))
    device.setGainMode(direction, 0, true)

    assertEqual(0.0, device.getGain(direction, 0))
    assertEqual(0.0, device.getGain(direction, 0, ""))
    device.setGain(direction, 0, 0.0)
    device.setGain(direction, 0, "", 0.0)

    device.getGainRange(direction, 0)
    device.getGainRange(direction, 0, "")

    #
    # Frequency API (TODO: why don't the overloads with tune args work?)
    #

    device.setFrequency(direction, 0, 0.0)
    device.setFrequency(direction, 0, 0.0, "key0=val0,key1=val1")
    assertEqual(0.0, device.getFrequency(direction, 0))

    device.setFrequency(direction, 0, "", 0.0)
    device.setFrequency(direction, 0, "", 0.0, "key0=val0,key1=val1")
    assertEqual(0.0, device.getFrequency(direction, 0, ""))

    assertEqual(0, length(device.listFrequencies(direction, 0)))

    assertEqual(0, length(device.getFrequencyRange(direction, 0)))
    assertEqual(0, length(device.getFrequencyRange(direction, 0, "")))
    assertEqual(0, length(device.getFrequencyArgsInfo(direction, 0)))

    #
    # Sample rate API
    #

    device.setSampleRate(direction, 0, 0.0)
    assertEqual(0.0, device.getSampleRate(direction, 0))
    assertEqual(0, length(device.getSampleRateRange(direction, 0)))

    #
    # Bandwidth API
    #

    device.setBandwidth(direction, 0, 0.0)
    assertEqual(0.0, device.getBandwidth(direction, 0))
    assertEqual(0, length(device.getBandwidthRange(direction, 0)))

    #
    # Sensor API
    #

    assertEqual(0, length(device.listSensors(direction, 0)))
    device.getSensorInfo(direction, 0, "")
    assertEqual("", device.readSensor(direction, 0, ""))

    #
    # Settings API
    #

    assertEqual(0, length(device.getSettingInfo(direction, 0)))
    device.getSettingInfo(direction, 0, "")
    assertEqual("", device.readSetting(direction, 0, ""))
    device.writeSetting(direction, 0, "", "")
    device.writeSetting(direction, 0, "", 0)
    device.writeSetting(direction, 0, "", 0.0)

function testDeviceAPI
    SoapySDR;

    device = SoapySDR.Device("driver=null,type=null")

    #
    # Identification API
    #

    assertEqual("null", device.getDriverKey())
    assertEqual("null", device.getHardwareKey())
    assertEqual("", device.getHardwareInfo())

    #
    # Clocking API
    #

    device.setMasterClockRate(0.0)
    assertEqual(0.0, device.getMasterClockRate())
    assertEqual(0, length(device.getMasterClockRates()))

    device.setReferenceClockRate(0.0)
    assertEqual(0.0, device.getReferenceClockRate())
    assertEqual(0, length(device.getReferenceClockRates()))

    device.setClockSource("")
    assertEqual("", device.getClockSource())
    assertEqual(0, length(device.listClockSources()))

    #
    # Time API
    #

    device.setTimeSource("")
    assertEqual("", device.getTimeSource())
    assertEqual(0, length(device.listTimeSources()))

    assertFalse(device.hasHardwareTime())
    assertFalse(device.hasHardwareTime(""))
    device.setHardwareTime(0)
    device.setHardwareTime(0, "")
    assertEqual(int64(0), device.getHardwareTime())
    assertEqual(int64(0), device.getHardwareTime(""))

    #
    # Sensor API
    #

    assertEqual(0, length(device.listSensors()))
    device.getSensorInfo("")
    assertEqual("", device.readSensor(""))

    #
    # Register API
    #

    device.listRegisterInterfaces()

    device.writeRegister("", 0, 0)
    assertEqual(0, device.readRegister("", 0))

    device.writeRegisters("", 0, [1, 2, 3])
    assertEqual(0, length(device.readRegisters("", 0, 0)))

    #
    # Settings API
    #

    assertEqual(0, length(device.getSettingInfo()))
    device.getSettingInfo("")
    assertEqual("", device.readSetting(""))
    device.writeSetting("", "")
    device.writeSetting("", 0)
    device.writeSetting("", 0.0)

    #
    # GPIO API
    #

    device.listGPIOBanks()
    device.writeGPIO("", 0)
    device.writeGPIO("", 0, 0)
    assertEqual(0, device.readGPIO(""))
    device.writeGPIODir("", 0)
    device.writeGPIODir("", 0, 0)
    assertEqual(0, device.readGPIODir(""))

    #
    # I2C API
    #

    device.writeI2C(0, "")
    assertEqual("", device.readI2C(0, 0))

    #
    # SPI API
    #

    assertEqual(0, device.transactSPI(0, 0, 0))

    #
    # UART API
    #

    device.listUARTs()
    device.writeUART("", "")
    assertEqual("", device.readUART(""))

    #
    # Direction-specific tests
    #

    testDirection(device, SoapySDR.Direction.TX)
    testDirection(device, SoapySDR.Direction.RX)
