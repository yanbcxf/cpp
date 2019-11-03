@echo off
rem Do not edit! This batch file is created by CASIO fx-9860G SDK.

if exist debug\*.obj  del debug\*.obj
if exist EDIT.G1A  del EDIT.G1A

cd debug
if exist FXADDINror.bin  del FXADDINror.bin
"C:\CASIO\fx-9860G SDK\OS\SH\Bin\Hmake.exe" Addin.mak
cd ..
if not exist debug\FXADDINror.bin  goto error

"C:\CASIO\fx-9860G SDK\Tools\MakeAddinHeader363.exe" "E:\github\fx9860gii\EDIT_v1.60\EDIT 2012.08.28 - Release v1.60\Source"
if not exist EDIT.G1A  goto error
echo Build has completed.
goto end

:error
echo Build was not successful.

:end

