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
rm -rf antimony.app
rm -rf antimony
rm -rf antimony.dmg

make -j8
macdeployqt antimony.app

cd antimony.app/Contents/PlugIns
rm -rf accessible audio imageformats mediaservice playlistformats position printsupport qml1tooling sensorgestures sensors

cd ../Frameworks
rm -rf QtDeclarative.framework QtMultimedia.framework QtMultimediaWidgets.framework QtPositioning.framework QtQml.framework QtQuick.framework QtScript.framework QtSensors.framework QtSql.framework QtXmlPatterns.framework
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
cp -r sb/fab antimony.app/Contents/Frameworks/Python.framework/Versions/3.4/lib/python3.4/fab
cp -r sb/nodes antimony.app/Contents/Resources

sed -i "" "s:0\.0\.0:$RELEASE:g" antimony.app/Contents/Info.plist

mkdir antimony
cp ../README.md ./antimony/README.txt
cp ../doc/USAGE.md ./antimony/USAGE.txt
mv antimony.app ./antimony
hdiutil create antimony.dmg -volname "Antimony $RELEASE" -srcfolder antimony
rm -rf antimony
