ilib_name = "SoapySDR_MEX";
table = ["SoapySDR_MEX", "SoapySDR_MEX", "cmex"];
files = ["@CMAKE_CURRENT_SOURCE_DIR@/../native/SoapySDR_MEX.cpp"];
libs = ["SoapySDR"];
ldflags = "";
cflags = "-DSOAPY_SDR_SCILAB -I@PROJECT_SOURCE_DIR@/include -I@PROJECT_SOURCE_DIR@/mex/native/include -L@PROJECT_BINARY_DIR@/lib -lSoapySDR";
fflags = "";

ilib_verbose(2);
ilib_mex_build(ilib_name,table,files,libs,"",ldflags,cflags,fflags);
