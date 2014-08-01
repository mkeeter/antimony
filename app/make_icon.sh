#!/bin/bash

INK=/Applications/Inkscape.app/Contents/Resources/bin/inkscape

if [[ -z "$1" ]] 
then
	echo "SVG file needed."
	exit;
fi

BASE=`basename "$1" .svg`
SVG="$1"

$INK -z -D -e "$BASE-16x.png" -f 	$SVG -w 16 -h 16
$INK -z -D -e "$BASE-32x.png" -f 	$SVG -w 32 -h 32
$INK -z -D -e "$BASE-128.png" -f 	$SVG -w 128 -h 128
$INK -z -D -e "$BASE-256.png" -f 	$SVG -w 256 -h 256
$INK -z -D -e "$BASE-512.png" -f 	$SVG -w 512 -h 512

png2icns sb.icns $BASE-*.png
