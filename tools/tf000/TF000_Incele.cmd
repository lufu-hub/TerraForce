@echo off
rem TerraForge TF-000: salt okunur ortam incelemesi. Hicbir proje dosyasini degistirmez.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Inspect-Environment.ps1" %*
pause
