

#define main groestlcoind_main

#include "groestlcoind.cpp"


extern "C" {

__declspec(dllexport) int GroestlcoindEntry() {
	int rc = groestlcoind_main(__argc, __argv);
	exit(rc);
	return rc;
}


} // C"
