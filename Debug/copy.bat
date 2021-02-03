@echo off
for /f "delims=" %%a in ('cscript -nologo 1.vbs') do set mtime=%%a
echo %mtime%
@pause