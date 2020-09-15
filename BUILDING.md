Requirements
------------
- [Qt 5.4 or later](http://www.qt.io/)
- [Python 3](https://www.python.org/)
- [Boost.Python](http://www.boost.org/doc/libs/1_57_0/libs/python/doc/index.html) (linked against Python 3)
- [`libpng`](http://www.libpng.org/pub/png/libpng.html)
- [Lemon](http://www.hwaci.com/sw/lemon/)
- [Flex](http://flex.sourceforge.net)
- [ninja](https://ninja-build.org/) (recommended)

--------------------------------------------------------------------------------

Mac OS X
--------
Tested on Mac OS X 10.13.4 with [homebrew](http://brew.sh/) already installed:
```
brew install libpng
brew install python3
brew install boost-python3
brew install qt5
brew install lemon
brew install flex
brew install ninja
brew install cmake

git clone https://github.com/mkeeter/antimony
cd antimony
mkdir build
cd build


cmake -DCMAKE_PREFIX_PATH=/usr/local/Cellar/qt/5.10.1 -GNinja ..
ninja

open app/Antimony.app
```

--------------------------------------------------------------------------------

Linux
-----
Tested on a clean Xubuntu 16.04 virtual machine:

```
# Install dependencies
sudo apt install git build-essential libpng-dev python3-dev libboost-all-dev  libgl1-mesa-dev lemon flex qt5-default ninja-build cmake

(On some distros, you may need to install `libqt5opengl5` or `libqt5opengl5-dev` as well).

# Clone the repo
git clone https://github.com/mkeeter/antimony
cd antimony

# Make a build folder
mkdir build
cd build

# Build and launch the application
cmake -GNinja ..
ninja
./app/antimony
```

To put Antimony on your path, call `sudo ninja install`.  This does two things:
- The `antimony` executable is copied to `/usr/local/bin`
- Antimony's Python libraries are copied to `/usr/local/share/antimony`

Debian packaging
----------------

Packaging for the Debian or flavors based on it can be done using the content of `deploy/linux/debian` directory.

In the project's root directory create a symbolic link:

```
ln -s deploy/linux/debian debian
```

Build the package:

```
dpkg-buildpackage -us -uc
```

At the end of this you'll find the packages in the directory containing the project's root directory. For example:

```
antimony_0.9.2_amd64.changes
antimony_0.9.2_amd64.deb
antimony_0.9.2.dsc
antimony_0.9.2.tar.xz
antimony-dbg_0.9.2_amd64.deb
```

To install the package, as root or using sudo:

```
dpkg --install antimony_0.9.2_amd64.deb
```

## Debugging
### `cannot find -lGL`
If running `make` gives the `/usr/bin/ld: cannot find -lGL`, create a symlink to the `libGL` file:
```
ln -s /usr/lib/x86_64-linux-gnu/mesa/libGL.so.1.2.0 /usr/lib/libGL.so
```

### Missing top menu
If the top menu bar is not appearing in Ubuntu with a non-Unity
desktop environment (e.g. `gnome-session-flashback`), run
```
sudo apt-get remove appmenu-qt5
```
to make it appear.

--------------------------------------------------------------------------------

Windows (experimental)
----------------------

Install [msys2-x86_84](http://msys2.github.io/)
and open an "MSYS2 MinGW 64-bit" shell

Run the following commands to install dependencies:
```
pacman -Syuu
pacman -S git make lemon flex mingw-w64-x86_64-python3 mingw-w64-x86_64-cmake mingw-w64-x86_64-qt5 mingw-w64-x86_64-toolchain
```

Sadly, we can't install Boost through `pacman` because of a
[packaging bug](https://github.com/Alexpux/MINGW-packages/issues/2028).
Instead, we'll build it by hand.

Download the latest [version of Boost](http://www.boost.org/users/download/)
and unzip it to your `mingw64` home directory
(which will be of the form `/mingw64/home/$USERNAME` in the MinGW64 shell, or
equivalently `C:\msys64\home\$USERNAME` in the Windows file explorer).

Then build Boost:
```
cd ~/boost_1_63_0
./bootstrap.sh
./b2 include=/mingw64/include/python3.5m --with-python
```

Rename the generated library to something that `cmake` will find:
```
cd stage/lib
mv libboost_python-mgw63-mt-1_63.a libboost_python3-mt.a
```

Then, run through the following steps to clone and build Antimony:
```
cd ~
git clone https://github.com/mkeeter/antimony
cd antimony
mkdir build
cd build
BOOST_ROOT=~/boost_1_63_0/ BOOST_LIBRARYDIR=~/boost_1_63_0/stage/ /mingw64/bin/cmake -G"MSYS Makefiles" ..
make -j8
```

To invoke Antimony from the build folder, call
```
PYTHONHOME=/mingw64 ./app/antimony.exe
```
