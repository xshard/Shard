// Copyright (c) 2014-2015 The Groestlcoin developers

#include "groestlcoin.h"


#include "hash.h"
#include "crypto/common.h"




using namespace std;

extern "C" {
#include <sphlib/sph_groestl.h>
} // "C"

namespace XCoin {

uint256 HashGroestl(const ConstBuf& cbuf) {
    sph_groestl512_context  ctx_gr[2];
    static unsigned char pblank[1];
    uint256 hash[4];
	
    sph_groestl512_init(&ctx_gr[0]);
    sph_groestl512 (&ctx_gr[0], cbuf.P ? cbuf.P : pblank, cbuf.Size);
    sph_groestl512_close(&ctx_gr[0], static_cast<void*>(&hash[0]));
	
	sph_groestl512_init(&ctx_gr[1]);
	sph_groestl512(&ctx_gr[1],static_cast<const void*>(&hash[0]), 64);
	sph_groestl512_close(&ctx_gr[1],static_cast<void*>(&hash[2]));
	
    return hash[2];
}

uint256 HashFromTx(const ConstBuf& cbuf) {
	CSHA256 sha;
	sha.Write(cbuf.P, cbuf.Size);
	uint256 r;
	sha.Finalize((unsigned char*)&r);
	return r;
}

uint256 HashForSignature(const ConstBuf& cbuf) {
	CSHA256 sha;
	sha.Write(cbuf.P, cbuf.Size);
	uint256 r;
	sha.Finalize((unsigned char*)&r);
	return r;
}


} // XCoin::

