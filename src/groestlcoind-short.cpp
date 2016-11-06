#include <windows.h>

#pragma comment(lib, "groestlcoin")

extern "C" {

__declspec(dllimport) int GroestlcoindEntry();



int MainEntry() {
	return GroestlcoindEntry();
}


} // "C"
