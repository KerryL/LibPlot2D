@ECHO OFF
REM This script generates a .cpp file containing the current git hash string

REM This is the file to which output will be written
SET outputPath="%1src/application/"
SET outputFile=gitHash.cpp
FOR /f %%i IN ('git rev-parse --short HEAD') DO SET gitHash=%%i

ECHO outputPath = %outputPath%
ECHO outputFile = %outputFile%
ECHO gitHash = %gitHash%

@ECHO // File:  %outputFile%> %outputPath%%outputFile%
@ECHO // Auth:  getGitHash.bat>> %outputPath%%outputFile%
@ECHO // Desc:  Automatically generated file containing latest git hash.>> %outputPath%%outputFile%
@ECHO.>> %outputPath%%outputFile%
@ECHO // Local headers>> %outputPath%%outputFile%
@ECHO #include "application/plotterApp.h">> %outputPath%%outputFile%
@ECHO.>> %outputPath%%outputFile%
@ECHO const wxString DataPlotterApp::gitHash = _T("%gitHash%");>> %outputPath%%outputFile%