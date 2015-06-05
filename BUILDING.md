Requirements
------------
- [Qt 5.4](http://www.qt.io/)
- [Python 3](https://www.python.org/)
- [Boost.Python](http://www.boost.org/doc/libs/1_57_0/libs/python/doc/index.html) (linked against Python 3)
- [`libpng`](http://www.libpng.org/pub/png/libpng.html)

Mac OS X
--------
Tested on Mac OS X 10.9.4 with [homebrew](http://brew.sh/) already installed:
```
brew install libpng
brew install python3
brew install --with-python3 boost-python
brew install qt5

git clone https://github.com/mkeeter/antimony
cd antimony
mkdir build
cd build

/usr/local/Cellar/qt5/5.4.*/bin/qmake ../qt/antimony.pro
make -j8

open antimony.app
```

Linux
-----
Tested on a clean Xubuntu 14.04 virtual machine:  
Install [Qt 5.4](http://www.qt.io/download-open-source/#section-3), then run
```
sudo apt-get install build-essential
sudo apt-get install libpng-dev
sudo apt-get install python3-dev
sudo apt-get install libboost-all-dev
sudo apt-get install libgl1-mesa-dev

git clone https://github.com/mkeeter/antimony
cd antimony
mkdir build
cd build

~/Qt5.4.0/5.4/gcc_64/bin/qmake ../qt/antimony.pro
make -j8

./antimony
```

You can use `make install`, or set up a symlink to run `antimony` from outside the build directory:
```
ln -s ~/antimony/build/antimony /usr/local/bin/antimony 
```

### Caveats

The path to `qmake` may vary depending on how Qt 5.4 was installed; if the above path doesn't work, try
```
~/Qt/5.4/gcc_64/bin/qmake ../qt/antimony.pro
```

--------------------------------------------------------------------------------

If running `make` gives the `/usr/bin/ld: cannot find -lGL`, create a symlink to the `libGL` file:
```
ln -s /usr/lib/x86_64-linux-gnu/mesa/libGL.so.1.2.0 /usr/lib/libGL.so
```

--------------------------------------------------------------------------------

If the top menu bar is not appearing in Ubuntu with a non-Unity
desktop environment (e.g. `gnome-session-flashback`), run
```
sudo apt-get remove appmenu-qt5
```
to make it appear.

Windows
-------

Building on Windows is a bit more challenging.  Here's the basic overview:
- Download and install Qt 5.4.2 (32-bit, MinGW)
- Download and install MinGW separately to get the MSYS tools
- Use MSYS to install `zlib` and `make`
- Download Boost 1.58 and unzip to `C:\boost_1_58_0`
- Build boost::python.  In Powershell, this looks like
```
cd boost_folder
$env:Path = $env:Path + ";C:\Qt\Qt5.4.2\Tools\mingw491_32\bin"
.\bootstrap.bat mingw
.\b2 --with-python toolset=gcc link=shared
```
- Download `libpng` and unzip to `C:\lpng1617`
- Compile `libpng` using the `MSYS` makefile.  In Powershell, this looks like
```
cd C:\lpng1617
cp scripts\makefile.msys makefile
$env:Path = $env:Path + ";C:\MinGW\msys\1.0\bin"
make
```

