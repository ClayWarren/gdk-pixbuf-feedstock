@echo on
cmake -S . -B consumer-build -G Ninja -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b 1
cmake --build consumer-build
if errorlevel 1 exit /b 1
consumer-build\gdk-pixbuf-consumer.exe
if errorlevel 1 exit /b 1

set "EXPECTED_MACHINE=8664 machine (x64)"
if "%target_platform%" == "win-arm64" set "EXPECTED_MACHINE=AA64 machine (ARM64)"
for %%F in ("consumer-build\gdk-pixbuf-consumer.exe" "%PREFIX%\Library\bin\gdk_pixbuf-2.0-0.dll" "%PREFIX%\Library\bin\gdk-pixbuf-csource.exe" "%PREFIX%\Library\bin\gdk-pixbuf-query-loaders.exe" "%PREFIX%\Library\bin\gdk-pixbuf-pixdata.exe") do (
    dumpbin /headers "%%~F" | findstr /c:"%EXPECTED_MACHINE%"
    if errorlevel 1 exit /b 1
)
for /r "%PREFIX%\Library\lib\gdk-pixbuf-2.0" %%F in (*.dll) do (
    dumpbin /headers "%%~F" | findstr /c:"%EXPECTED_MACHINE%"
    if errorlevel 1 exit /b 1
)
