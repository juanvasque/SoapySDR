cd("/home/ncorgan/dev/sdr/src/SoapySDR/mex/scilab")

ilib_mex_build( ..
    "SoapySDR_MEX", ..
    ["SoapySDR_MEX", "SoapySDR_MEX", "cmex"], ..
    ["../native/SoapySDR_MEX.cpp"], ..
    ["SoapySDR"], ..
    "", ..
    "-L/usr/local/lib", ..
    "-I/usr/local/include -I/home/ncorgan/dev/sdr/src/SoapySDR/mex/native/include", ..
    "", ..
    "")
