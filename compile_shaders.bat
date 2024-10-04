@echo off
setlocal

echo Compiling shaders from %1

set FXC="C:\Program Files (x86)\Windows Kits\10\bin\10.0.22621.0\x64\fxc.exe"
set SHADER_DIR=%1\VamosGameEngine\Engine\Data\Shaders
set OUTPUT_DIR=%1\VamosGameEngine\Engine\Data\CompiledShaders

if not exist "%SHADER_DIR%" (
    echo Shader directory "%SHADER_DIR%" does not exist.
    exit /b 1
)

if not exist "%OUTPUT_DIR%" (
    mkdir "%OUTPUT_DIR%"
)

for %%f in ("%SHADER_DIR%\*.hlsl") do (
    echo Compiling %%~nf
    %FXC% /T vs_5_0 /E VSMain /Fo "%OUTPUT_DIR%\%%~nf_vs.cso" "%%f"
    %FXC% /T ps_5_0 /E PSMain /Fo "%OUTPUT_DIR%\%%~nf_ps.cso" "%%f"
)

endlocal
