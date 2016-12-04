// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2014-2015 The Groestlcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "amount.h"
#include "uint256.h"


namespace XCoin {

class ConstBuf {
public:
	const unsigned char *P;
	size_t Size;

	template <typename T>
	ConstBuf(const T pb, const T pe) {
		if (pb == pe) {
			P = 0;
			Size = 0;
		} else {
			P = (unsigned char*)(&pb[0]);
			Size = (pe-pb) * sizeof(pb[0]);
		}
	}

	ConstBuf(const std::vector<unsigned char>& vch) {
		if (vch.empty()) {
			P = 0;
			Size = 0;
		} else {
			P = &vch[0];
			Size = vch.size();
		}
	}
};


uint256 HashGroestl(const ConstBuf& cbuf);

uint256 HashFromTx(const ConstBuf& cbuf);
uint256 HashForSignature(const ConstBuf& cbuf);
inline uint256 HashPow(const ConstBuf& cbuf) { return HashGroestl(cbuf); }
inline uint256 HashMessage(const ConstBuf& cbuf) { return HashGroestl(cbuf); }
inline uint256 HashForAddress(const ConstBuf& cbuf) { return HashGroestl(cbuf); }


class MessageHasher {
private:
	void *ctx;
public:
	void Finalize(unsigned char hash[32]);
	MessageHasher& Write(const unsigned char *data, size_t len);
	MessageHasher();
	MessageHasher(MessageHasher&& x);
	~MessageHasher();
	MessageHasher& operator=(MessageHasher&& x);
};


} // XCoin::

