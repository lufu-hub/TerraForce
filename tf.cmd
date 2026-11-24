@echo off
rem TerraForge komutlari. Ornek: tf build   /   tf help
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Scripts\tf.ps1" %*
exit /b %ERRORLEVEL%
