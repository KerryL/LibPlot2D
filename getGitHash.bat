@ECHO OFF
REM This script generates a .cpp file containing the current git hash string

REM This is the file to which output will be written
SET outputPath="%1src/"
SET outputFile=gitHash.cpp
FOR /f %%i IN ('git describe --tags --abbrev^=0') DO SET gitTag=%%i
FOR /f %%i IN ('git rev-parse --short HEAD') DO SET gitHash=%%i

REM ECHO outputPath = %outputPath%
REM ECHO outputFile = %outputFile%
ECHO gitTag = %gitTag%
ECHO gitHash = %gitHash%

@ECHO // File:  %outputFile%> %outputPath%%outputFile%
@ECHO // Auth:  getGitHash.bat>> %outputPath%%outputFile%
@ECHO // Desc:  Automatically generated file containing latest git hash.>> %outputPath%%outputFile%
@ECHO.>> %outputPath%%outputFile%
@ECHO // Local headers>> %outputPath%%outputFile%
@ECHO #include "lp2d/libPlot2D.h">> %outputPath%%outputFile%
@ECHO.>> %outputPath%%outputFile%
@ECHO const wxString LibPlot2D::versionString = _T("%gitTag%");>> %outputPath%%outputFile%
@ECHO const wxString LibPlot2D::gitHash = _T("%gitHash%");>> %outputPath%%outputFile%
