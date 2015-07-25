#	Build MSI packages

WIX_LINK = light.exe -ext WixUIExtension -ext WixUtilExtension -dWixUILicenseRtf=doc\gplv3.rtf

msi : groestlcoin_x86.msi groestlcoin_x64.msi

groestlcoin_x86.msi : groestlcoin.wxs
	candle.exe -o groestlcoin-x86.wixobj groestlcoin.wxs
	$(WIX_LINK)  -out $@ groestlcoin-x86.wixobj

groestlcoin_x64.msi : groestlcoin.wxs
	candle.exe -arch x64 -o groestlcoin-x64.wixobj groestlcoin.wxs
	$(WIX_LINK) -out $@ groestlcoin-x64.wixobj


