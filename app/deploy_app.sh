#!/bin/sh
set -x -e

cd ../build-$1
make clean
rm -rf antimony.app

make
macdeployqt antimony.app

cd antimony.app/Contents/PlugIns
rm -rf accessible audio imageformats mediaservice playlistformats position printsupport qml1tooling sensorgestures sensors

cd ../Frameworks
rm -rf QtDeclarative.framework QtMultimedia.framework QtMultimediaWidgets.framework QtNetwork.framework QtPositioning.framework QtQml.framework QtQuick.framework QtScript.framework QtSensors.framework QtSql.framework QtXmlPatterns.framework
cp -r /usr/local/Frameworks/Python.framework .
install_name_tool -change /usr/local/Frameworks/Python.framework/Versions/3.4/Python \
                         @executable_path/../Frameworks/Python.framework/Versions/3.4/Python \
                         libboost_python3.dylib

cd ../Resources
rm empty.lproj

cd ../MacOS
install_name_tool -change /usr/local/Frameworks/Python.framework/Versions/3.4/Python \
                         @executable_path/../Frameworks/Python.framework/Versions/3.4/Python \
                         antimony
