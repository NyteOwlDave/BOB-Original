@echo off

rem --- Creates a Folder Named BACKUP
rem --- Copies all relevant source files
rem ----and text files to this folder.

set PROJECT=BOB-Original
set DST=%PROJECT%-BACKUP

title %PROJECT% Backup

if exist "%DST%" goto NoNeedToCreateFolder
echo Creating %DST% Folder...
md %DST%
if errorlevel 1 goto BummerDude

:NoNeedToCreateFolder

echo.
echo =========================================================================
echo Backing up C files
echo =========================================================================
copy *.c %DST%
echo.

echo.
echo =========================================================================
echo Backing up H files
echo =========================================================================
copy *.h %DST%
echo.

echo.
echo =========================================================================
echo Backing up TXT files
echo =========================================================================
copy *.txt %DST%
echo.

echo.
echo =========================================================================
echo Backing up CMD files
echo =========================================================================
copy *.cmd %DST%
echo.

echo.
echo =========================================================================
echo Backing up Visual Studio Project files
echo =========================================================================
copy %PROJECT%.* %DST%
echo.

echo.
echo =========================================================================
echo Backup complete
echo =========================================================================
echo.
echo Consider zipping the folder and saving it elsewhere
echo so it won't be overwritten next time!
echo.

:ByeBye

pause

set DST=
set PROJECT=

exit /b

:BummerDude

echo Unable to create the folder, dang it!
echo.

goto ByeBye

