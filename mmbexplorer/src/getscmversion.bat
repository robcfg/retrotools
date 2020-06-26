@echo off
for /f %%i in ('git rev-parse --short HEAD') do echo const char^* scmVersion=^"%%i^"^;>scmversion.h