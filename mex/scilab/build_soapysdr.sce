cd("/home/ncorgan/dev/sdr/src/SoapySDR/mex/scilab")

ilib_verbose(2)
ilib_mex_build( ..
    "SoapySDR_MEX", ..
    ["SoapySDR_MEX", "SoapySDR_MEX", "cmex"], ..
    ["../native/SoapySDR_MEX.cpp"], ..
    ["SoapySDR"], ..
    "", ..
    "-L/usr/local/lib -lSoapySDR", ..
    "-DSOAPY_SDR_SCILAB -I/usr/local/include -I/home/ncorgan/dev/sdr/src/SoapySDR/mex/native/include", ..
    "", ..
    "")

exec("loader.sce")
SoapySDR_MEX("Version_getABIVersion")
