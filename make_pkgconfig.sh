#!/bin/bash
# File:  make_pkgconfig.sh
# Date:  12/14/2016
# Auth:  K. Loux
# Desc:  This file creates a pkg-config pc file.

incDir="$1"
libDir="$2"
outputFile="$3"
debug="$4"

# Get the version from the most recent git tag
version=$(git describe --tags --abbrev=0)

if [ "$debug" -eq "0" ] ; then
	debug_flag="--debug=no"
else
	debug_flag="--debug=yes"
fi

echo prefix=/usr> $outputFile
echo exec_prefix=/usr>> $outputFile
echo libdir=$libDir>> $outputFile
echo includedir=$incDir>> $outputFile
echo>> $outputFile
echo Name: LibPlot2D>> $outputFile
echo URL: https://github.com/KerryL/LibPlot2D>> $outputFile
echo Description: Lightweight 2D plotting library.>> $outputFile
echo Version: $version>> $outputFile
echo Requires: freetype2,glew,gl>> $outputFile
echo Requires.private: >> $outputFile
echo Libs: -L\${libdir} -llibPlot2D \`wx-config --version=3.1 $debug_flag --libs all\`>> $outputFile
echo Libs.private: >> $outputFile
echo Cflags: -I\${includedir} \`wx-config --version=3.1 $debug_flag --cppflags\` -std=c++14>> $outputFile