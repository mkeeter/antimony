#!/bin/sh
set -x -e

if [ $# -ne 1 ]; then
    echo "Usage: deploy_app.sh (debug|release)"
    exit 1
fi

RELEASE=`git describe --exact-match --tags || echo "($(git rev-parse --abbrev-ref HEAD))"`

cd ../../build-$1
make clean
make qmake
rm -rf app lib

make -j8
macdeployqt app/Antimony.app

cd app/Antimony.app/Contents/PlugIns
rm -rf accessible audio imageformats mediaservice playlistformats position printsupport qml1tooling sensorgestures sensors

cd ../Frameworks
rm -rf QtDeclarative.framework QtMultimedia.framework QtMultimediaWidgets.framework QtPositioning.framework QtQml.framework QtQuick.framework QtScript.framework QtSensors.framework QtSql.framework QtXmlPatterns.framework Qt3DCore.framework Qt3DRender.framework QtLocation.framework QtSerialBus.framework QtSerialPort.framework

PY_VERSION=3.5
cp -r /usr/local/Frameworks/Python.framework .
rm -rf Python.framework/Versions/2.7 Python.framework/Versions/Current
ln -s Python.framework/Versions/$PY_VERSION Python.framework/Versions/Current
rm -rf Python.framework/Current/lib/python$PY_VERSION/site-packages/*
chmod a+w libboost_python3.dylib

cd ../Resources
rm empty.lproj

cd ../MacOS
install_name_tool -change /usr/local/opt/python3/Frameworks/Python.framework/Versions/$PY_VERSION/Python \
                         @executable_path/../Frameworks/Python.framework/Versions/$PY_VERSION/Python \
                         Antimony

cd ../../..
cp -r sb/fab Antimony.app/Contents/Frameworks/Python.framework/Versions/$PY_VERSION/lib/python$PY_VERSION/fab
cp -r sb/nodes Antimony.app/Contents/Resources

sed -i "" "s:0\.0\.0:$RELEASE:g" Antimony.app/Contents/Info.plist

mkdir Antimony
cp ../../README.md ./Antimony/README.txt
cp ../../doc/USAGE.md ./Antimony/USAGE.txt
cp ../../doc/SCRIPTING.md ./Antimony/SCRIPTING.txt
mv Antimony.app ./Antimony
hdiutil create Antimony.dmg -volname "Antimony $RELEASE" -srcfolder Antimony
rm -rf Antimony
mv Antimony.dmg ..
