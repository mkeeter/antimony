Mac OS X
--------
Tested on Mac OS X 10.9.4 with [homebrew](http://brew.sh/) already installed:
```
brew install python3
brew install --c++11 --with-python3 boost
brew install qt5
git clone https://github.com/mkeeter/antimony
cd antimony
mkdir build
cd build
/usr/local/Cellar/qt5/*/bin/qmake ../qt/antimony.pro
make -j8
open antimony.app
```

Linux
-----
Tested on a clean Xubuntu 14.04 virtual machine:
```
sudo apt-get install qt5-default qt5-qmake
sudo apt-get install python3-dev
sudo apt-get install libboost-all-dev
git clone https://github.com/mkeeter/antimony
cd antimony
mkdir build
qmake ../qt/antimony.pro
make -j8
./antimony
```

