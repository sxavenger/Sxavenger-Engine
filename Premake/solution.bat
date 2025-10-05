cd /d "%~dp0"

premake5 --file=premake5_vs2022.lua vs2022

echo Premake5 solution generation complete.
pause