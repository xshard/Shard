// Copyright (c) 2014-2015 The GroestlCoin developers

#include "groestlcoin.h"

#include <boost/assign/list_of.hpp>

#include "arith_uint256.h"
#include "chain.h"
#include "chainparams.h"
#include "consensus/params.h"
#include "utilstrencodings.h"
#include "crypto/sha256.h"

#include "bignum.h"

extern "C" {
#include <sphlib/sph_groestl.h>

#if !defined(UCFG_LIBEXT) && (defined(_M_IX86) || defined(_M_X64))

	static __inline void Cpuid(int a[4], int level) {
#	ifdef _MSC_VER
		__cpuid(a, level);
#	else
		__cpuid(level, a[0], a[1], a[2], a[3]);
#	endif
	}

	char g_bHasSse2;

	static int InitBignumFuns() {
		int a[4];
		::Cpuid(a, 1);
		g_bHasSse2 = a[3] & 0x02000000;
		return 1;
	}

	static int s_initBignumFuns = InitBignumFuns();
#endif // defined(_M_IX86) || defined(_M_X64)
}

using namespace std;


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

