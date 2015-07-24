#pragma once

#include "uint256.h"

static const int64 MAX_MONEY = 105000000 * COIN;

namespace XCoin {

class ConstBuf {
	const unsigned char *P;
	size_t Size;

	template <typename T>
	ConstBuf(const T pb, const T pe) {
		if (pb == pe) {
			P = 0;
			Size = 0;
		} else {
			P = &pb[0];
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

inline uint256 HashMessage(const ConstBuf& cbuf) { return HashGroestl(cbuf); }
inline uint256 HashForAddress(const ConstBuf& cbuf) { return HashGroestl(cbuf); }




} // XCoin::

