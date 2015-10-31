Requirements
------------
- [Qt 5.4](http://www.qt.io/)
- [Python 3](https://www.python.org/)
- [Boost.Python](http://www.boost.org/doc/libs/1_57_0/libs/python/doc/index.html) (linked against Python 3)
- [`libpng`](http://www.libpng.org/pub/png/libpng.html)
- [Lemon](http://www.hwaci.com/sw/lemon/)
- [Flex](http://flex.sourceforge.net)

Mac OS X
--------
Tested on Mac OS X 10.9.4 with [homebrew](http://brew.sh/) already installed:
```
brew install libpng
brew install python3
brew install --with-python3 boost-python
brew install qt5
brew install lemon
brew install flex

git clone https://github.com/mkeeter/antimony
cd antimony
mkdir build
cd build

/usr/local/Cellar/qt5/5.*/bin/qmake ../sb.pro
make -j8

open app/Antimony.app
```
Note: If `make -j8` exits with an "Error 2" just run `make -j8` again to succeed.

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
sudo apt-get install lemon
sudo apt-get install flex
```

Because 14.04 ships with gcc 4.8, you'll need to update the compiler manually:
```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install gcc-4.9 g++-4.9
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.9
```

Then, you can clone and build Antimony.  Building from the `develop` branch is recommended over `master`, as `develop` has all of the latest bug-fixes and improvements.
```
git clone https://github.com/mkeeter/antimony
cd antimony
mkdir build
cd build

~/Qt5.4.0/5.4/gcc_64/bin/qmake ../sb.pro
make -j8

./app/antimony
```

You can use `make install`, or set up a symlink to run `antimony` from outside the build directory:
```
ln -s ~/antimony/build/app/antimony /usr/local/bin/antimony
```

### Caveats

The path to `qmake` may vary depending on how Qt 5.4 was installed; if the above path doesn't work, try
```
~/Qt/5.4/gcc_64/bin/qmake ../sb.pro
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

