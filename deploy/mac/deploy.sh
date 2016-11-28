#!/bin/sh
set -x -e

if [ $# -ne 1 ]; then
    echo "Usage: deploy_app.sh (debug|release)"
    exit 1
fi

RELEASE=`git describe --exact-match --tags || echo "($(git rev-parse --abbrev-ref HEAD))"`
RELEASE=`echo $RELEASE|sed s:/:-:g`

cd ../../build-$1
ninja clean
ninja

# Pull out framework paths info with otool
MACDEPLOYQT=`otool -L app/Antimony.app/Contents/MacOS/Antimony | sed -n -e "s:\(.*\)lib/QtCore.*:\1/bin/macdeployqt:gp"`
PY3_VERSION=`otool -L app/Antimony.app/Contents/MacOS/Antimony | sed -n -e "s:.*Python.framework/Versions/\(3\..\).*:\1:gp"`
PY3_FRAMEWORK=`otool -L app/Antimony.app/Contents/MacOS/Antimony | sed -n -e "s:\(.*Python.framework\)/Versions.*:\1:gp"`
BOOST_DYLIB=`otool -L app/Antimony.app/Contents/MacOS/Antimony | sed -n -e "s:.*\(libboost.*python.*\.dylib\).*:\1:gp"`

$MACDEPLOYQT app/Antimony.app

# Delete unused Qt plugins
cd app/Antimony.app/Contents/PlugIns
rm -rf accessible audio imageformats mediaservice playlistformats position printsupport qml1tooling sensorgestures sensors bearer

fix_qt () {
    echo "Fixing Qt for $1"
    for LIB in $( otool -L $1 | sed -n -e "s:\(.*Qt.*.framework[^ ]*\).*:\1:gp" )
    do
        RENAMED=`echo $LIB | sed -n -e "s:.*\(Qt.*\)\.framework.*:@executable_path/../Frameworks/\1.framework/Versions/5/\1:gp"`
        install_name_tool -change $LIB $RENAMED $1
    done
}

# Remap platform links
cd platforms
fix_qt libqcocoa.dylib

# Delete unused Qt frameworks
cd ../../Frameworks
rm -rf QtDeclarative.framework QtMultimedia.framework QtMultimediaWidgets.framework QtPositioning.framework QtQml.framework QtQuick.framework QtScript.framework QtSensors.framework QtSql.framework QtXmlPatterns.framework Qt3DCore.framework Qt3DRender.framework QtLocation.framework QtSerialBus.framework QtSerialPort.framework

# Clean up remaining Qt frameworks
for LIB in $( ls|sed -n -e "s:\(Qt.*\)\.framework:\1:gp" )
do
    fix_qt $LIB.framework/Versions/Current/$LIB
done

# Deploy the Python framework, cleaning out older versions and site-packages
rm -rf Python.framework
cp -R  $PY3_FRAMEWORK .
rm -rf Python.framework/Versions/2.7
rm -r  Python.framework/Versions/Current
rm -rf Python.framework/Versions/$PY3_VERSION/lib/python$PY3_VERSION/site-packages/*
rm -r  Python.framework/Versions/$PY3_VERSION/lib/python$PY3_VERSION/test
rm -r  Python.framework/Versions/$PY3_VERSION/lib/python$PY3_VERSION/__pycache__
rm -rf Python.framework/Versions/$PY3_VERSION/lib/python$PY3_VERSION/*/__pycache__
rm -r  Python.framework/Versions/$PY3_VERSION/Resources/English.lproj
rm -r  Python.framework/Versions/$PY3_VERSION/share/doc
chmod a+w $BOOST_DYLIB

# Rewire Python framework to point into the application bundle
cd ../MacOS
install_name_tool -change $PY3_FRAMEWORK/Versions/$PY3_VERSION/Python \
                          @executable_path/../Frameworks/Python.framework/Versions/$PY3_VERSION/Python \
                          Antimony

# Update release number in Info.plist
cd ../../..
sed -i "" "s:0\.0\.0:$RELEASE:g" Antimony.app/Contents/Info.plist

# Create the disk image
mkdir Antimony
cp ../../README.md ./Antimony/README.txt
cp ../../doc/USAGE.md ./Antimony/USAGE.txt
cp ../../doc/SCRIPTING.md ./Antimony/SCRIPTING.txt
cp -R Antimony.app ./Antimony
hdiutil create Antimony.dmg -volname "Antimony $RELEASE" -srcfolder Antimony
rm -rf Antimony
mv Antimony.dmg ..
