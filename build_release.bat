@echo off
echo Building RegExpCalc64...
"C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" RegExpCalc.sln /p:Configuration=Release /p:Platform=x64 /t:Rebuild /v:minimal
if %ERRORLEVEL% EQU 0 (
    echo Build succeeded!
) else (
    echo Build failed with error %ERRORLEVEL%
)
echo Building RegExpCalc32...
"C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" RegExpCalc.sln /p:Configuration=Release /p:Platform=x86 /t:Rebuild /v:minimal
if %ERRORLEVEL% EQU 0 (
    echo Build succeeded!
) else (
    echo Build failed with error %ERRORLEVEL%
)
