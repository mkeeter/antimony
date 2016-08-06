#!/bin/bash
set -e

INKPATH=(
    /Applications/Inkscape.app
    /opt/homebrew-cask/Caskroom/inkscape/0.48.5-2/Inkscape.app
    /opt/homebrew-cask/Caskroom/inkscape/0.91-1/Inkscape.app
)

for i in "${INKPATH[@]}"
do
    bin=$i/Contents/Resources/bin/inkscape
    if [ -e $bin ]; then
        INK=$bin
        break
    fi
done

if [ -z "$INK" ]; then
    echo "Could not find Inkscape executable."
    exit
fi

if [[ -z "$1" ]]
then
    echo "SVG file needed."
    exit
fi

BASE=`basename "$1" .svg`
DIR=`dirname "$1"`
SVG="$1"

$INK -z -D -e "$DIR/$BASE-16x.png" -f $SVG -w 16 -h 16
$INK -z -D -e "$DIR/$BASE-32x.png" -f $SVG -w 32 -h 32
$INK -z -D -e "$DIR/$BASE-128.png" -f $SVG -w 128 -h 128
$INK -z -D -e "$DIR/$BASE-256.png" -f $SVG -w 256 -h 256
$INK -z -D -e "$DIR/$BASE-512.png" -f $SVG -w 512 -h 512

png2icns sb.icns $DIR/$BASE-*.png
