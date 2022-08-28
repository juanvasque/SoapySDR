cd("/home/ncorgan/dev/sdr/src/SoapySDR/mex/scilab")

ilib_name = "SoapySDR_MEX";
table = ["SoapySDR_MEX", "SoapySDR_MEX", "cmex"];
files = ["../native/SoapySDR_MEX.cpp"];
libs = ["SoapySDR"];
ldflags = "";
cflags = "-DSOAPY_SDR_SCILAB -I/usr/local/include -I/home/ncorgan/dev/sdr/src/SoapySDR/mex/native/include -L/usr/local/lib -lSoapySDR";
fflags = "";

ilib_verbose(2);
ilib_mex_build(ilib_name,table,files,libs,"",ldflags,cflags,fflags);

exec("loader.sce");
SoapySDR_MEX("Version_getABIVersion");
