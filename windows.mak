#	Build MSI packages

MSB=%ProgramFiles(x86)%\MSBuild\14.0\Bin\amd64\msbuild
WIX_LINK = light.exe -ext WixUIExtension -ext WixUtilExtension -dWixUILicenseRtf=doc\gplv3.rtf

all : groestlcoin_x86.msi groestlcoin_x64.msi

src\qt\res_bitcoin.cpp : src\qt\moc.proj
	cd src\qt
	msbuild moc.proj
	cd ..\..


x86_R_St\groestlcoin-qt.exe : src\qt\res_bitcoin.cpp
	$(MSB) groestlcoin.sln /p:Configuration=R_St,Platform=x86 /v:n

x64_R_St\groestlcoin-qt.exe : src\qt\res_bitcoin.cpp
	$(MSB) groestlcoin.sln /p:Configuration=R_St,Platform=x64 /v:n


groestlcoin_x86.msi : groestlcoin.wxs x86_R_St\groestlcoin-qt.exe
	candle.exe -o groestlcoin-x86.wixobj groestlcoin.wxs
	$(WIX_LINK)  -out $@ groestlcoin-x86.wixobj

groestlcoin_x64.msi : groestlcoin.wxs x64_R_St\groestlcoin-qt.exe
	candle.exe -arch x64 -o groestlcoin-x64.wixobj groestlcoin.wxs
	$(WIX_LINK) -out $@ groestlcoin-x64.wixobj


