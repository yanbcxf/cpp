@echo off
rem Do not edit! This batch file is created by CASIO fx-9860G SDK.


if exist HELLO.G1A  del HELLO.G1A

cd debug
if exist FXADDINror.bin  del FXADDINror.bin
"C:\CASIO\fx-9860G SDK\OS\SH\Bin\Hmake.exe" Addin.mak
cd ..
if not exist debug\FXADDINror.bin  goto error

"C:\CASIO\fx-9860G SDK\Tools\MakeAddinHeader363.exe" "E:\github\fx9860gii\hello"
if not exist HELLO.G1A  goto error
echo Build has completed.
goto end

:error
echo Build was not successful.

:end

