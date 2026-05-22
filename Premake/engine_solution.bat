cd /d "%~dp0"

premake5 --version
premake5 --file=engine_premake5_vs2026.lua vs2026

echo Premake5 solution generation complete.

timeout /t 3 > nul
exit