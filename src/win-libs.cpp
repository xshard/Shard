#pragma comment(lib, "user32")
#pragma comment(lib, "shlwapi")
#pragma comment(lib, "crypt32")

#pragma comment(lib, "openssl")
#pragma comment(lib, "mpir")
#pragma comment(lib, "leveldb")

#pragma comment(lib, "groestlcoin-common")

#ifdef USE_QT
#	pragma comment(lib, "imm32")
#	pragma comment(lib, "winmm")

#	pragma comment(lib, "protobuf")

#	include <QtPlugin>

Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)

#	pragma comment(lib, "Qt5Core")
#	pragma comment(lib, "Qt5Network")
#	pragma comment(lib, "Qt5Gui")
#	pragma comment(lib, "Qt5Widgets")
#	pragma comment(lib, "Qt5PlatformSupport.lib")
#	pragma comment(lib, "qtharfbuzzng")
#	pragma comment(lib, "qtmain.lib")
#	pragma comment(lib, "qwindows.lib")
#endif

#include <windows.h>
#undef WinMain

extern "C" {

	int WINAPI WinMain(
		_In_ HINSTANCE hInstance,
		_In_ HINSTANCE hPrevInstance,
		_In_ LPSTR     lpCmdLine,
		_In_ int       nCmdShow
		);

	int WINAPI wWinMain(
		_In_ HINSTANCE hInstance,
		_In_ HINSTANCE hPrevInstance,
		_In_ LPWSTR     lpCmdLine,
		_In_ int       nCmdShow
		) {

		return WinMain(hInstance, hPrevInstance, 0, nCmdShow);


	}
} // "C"
