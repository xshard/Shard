#	Build MSI packages

MSB=%ProgramFiles(x86)%\MSBuild\14.0\Bin\amd64\msbuild.exe
WIX_LINK = light.exe -ext WixUIExtension -ext WixUtilExtension -dWixUILicenseRtf=doc\shard_lic.rtf -dWixUIDialogBmp=share\pixmaps\wix-banner.bmp -dWixUIBannerBmp=share\pixmaps\wix-topbanner.bmp


all : shard_x86.msi shard_x64.msi

src\qt\res_bitcoin.cpp : src\qt\moc.proj
	cd src\qt
	msbuild moc.proj
	cd ..\..


x86_R_St\shard-qt.exe : src\shard.cpp src\qt\res_bitcoin.cpp
	""$(MSB)"" shard.sln /p:Configuration=R_St,Platform=x86 /v:n

x64_R_St\shard-qt.exe : src\shard.cpp src\qt\res_bitcoin.cpp
	""$(MSB)"" shard.sln /p:Configuration=R_St,Platform=x64 /v:n


shard_x86.msi : shard.wxs x86_R_St\shard-qt.exe
	candle.exe -o shard-x86.wixobj shard.wxs
	$(WIX_LINK)  -out $@ shard-x86.wixobj

shard_x64.msi : shard.wxs x64_R_St\shard-qt.exe
	candle.exe -arch x64 -o shard-x64.wixobj shard.wxs
	$(WIX_LINK) -out $@ shard-x64.wixobj
