#!/bin/sh
set -x -e

if [ $# -ne 1 ]; then
    echo "Usage: deploy_app.sh (debug|release)"
    exit 1
fi

cd ../build-$1
make clean
make qmake
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

cd ../../..
cp -r fab antimony.app/Contents/Frameworks/Python.framework/Versions/3.4/lib/python3.4/fab

cp ../README.md .
tar -cvzf antimony.tar.gz antimony.app README.md
rm README.md

if [ `whoami` = "mkeeter" ]; then
    scp antimony.tar.gz mattkeeter.com:mattkeeter.com/projects/antimony/antimony.tar.gz
fi
