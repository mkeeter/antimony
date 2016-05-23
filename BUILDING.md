Requirements
------------
- [Qt 5.4 or later](http://www.qt.io/)
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
### Troubleshooting
If you have installed Homebrew in a non-standard directory like `~/.homebrew`
(the default is `/usr/local`), you'll need to provide a path to your Homebrew
files:
```
export BREW_HOME=/Users/yourusername/.homebrew

$BREW_HOME/Cellar/qt5/5.*/bin/qmake ../qt/antimony.pro
make -j8
```

If you need to further adjust your library paths, Mac specific settings
can be found in `qt/libpng.pri` and `qt/python.pri`

Note: If `make -j8` exits with an "Error 2" just run `make -j8` again to succeed.

Linux
-----
Tested on a clean Xubuntu 16.04 virtual machine:

```
# Install dependencies
sudo apt install git build-essential libpng-dev python3-dev libboost-all-dev  libgl1-mesa-dev lemon flex qt5-default

# Clone the repo
git clone https://github.com/mkeeter/antimony
cd antimony

# Make a build folder
mkdir build
cd build

# Build and launch the application
qmake ../sb.pro
make -j8
./app/Antimony
```

You can use `make install`, or set up a symlink to run `antimony` from outside the build directory:
```
ln -s ~/antimony/build/app/antimony /usr/local/bin/antimony
```

### Caveats

The path to `qmake` may vary depending on how Qt 5 was installed; if the instructions don't work, try
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
