#!/bin/bash
# This file automatically generates a source file containing git version
# and hash information.

outputPath="$1src/"
outputFile=gitHash.cpp

gitTag=$(git describe --tags --abbrev=0)
gitHash=$(git rev-parse --short HEAD)

#echo outputPath = $outputPath
#echo outputFile = $outputFile
echo version = $gitTag
echo gitHash = $gitHash

echo // File:  $outputFile> $outputPath$outputFile
echo // Auth:  getGitHash.sh>> $outputPath$outputFile
echo // Desc:  Automatically generated file containing latest git hash and version info.>> $outputPath$outputFile
echo>> $outputPath$outputFile
echo // Local headers>> $outputPath$outputFile
echo \#include \"lp2d/libPlot2D.h\">> $outputPath$outputFile
echo>> $outputPath$outputFile
echo const wxString LibPlot2D::versionString = _T\(\"$gitTag\"\)\;>> $outputPath$outputFile
echo const wxString LibPlot2D::gitHash = _T\(\"$gitHash\"\)\;>> $outputPath$outputFile
