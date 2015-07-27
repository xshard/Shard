Windows BUILD NOTES
====================
Some notes on how to build GroestlCoin in Windows. 

Tools
---------------------
Install Visual Studio 2015
Boost
Qt

Preparation
----------------
Build Boost and Qt as static libs


Get sources with 3rd-party libs
-----------------------------------
git clone --recursive https://github.com/GroestlCoin/bitcoin groestlcoin

cd groestlcoin
mkdir x86_libs
mkdir x64_libs
# copy built x86 Qt and boost .lib files to x86_libs/
# copy built x64 Qt and boost .lib files to x64_libs/


cd src\qt
msbuild moc.proj
cd ..

Open solution file groestlcoin.sln 
Select "R_St" configuration and one of platforms: x86 or x64
build



 