# SuperChat

Requirements:
*   gcc
*   boost library
*   ncurses library
*   OpenSplice

Installation Instructions:
```
sudo apt-get install libboost-all-dev
sudo apt-get install libncurses5-dev libncursesw5-dev
git clone https://github.com/kngu9/SuperChat3310
cd SuperChat3310
source scripts/release.com
make clean && make
./bin/SuperChat
```

Testing Instructions:
```
cd SuperChat3310
make test
```
