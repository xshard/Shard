Groestlcoin Core version 2.13.3 is now available from:

  <https://groestlcoin.org/downloads/>

This is a new major version release, including new features, various bugfixes and
performance improvements, as well as updated translations. It is recommended to upgrade to this version as soon as possible.

Please report bugs using the issue tracker at github:

  <https://github.com/groestlcoin/groestlcoin/issues>

Compatibility
==============

Microsoft ended support for Windows XP on [April 8th, 2014](https://www.microsoft.com/en-us/WindowsForBusiness/end-of-xp-support),
an OS initially released in 2001. This means that not even critical security
updates will be released anymore. Without security updates, using a groestlcoin
wallet on a XP machine is irresponsible at least.

We do not have time nor resources to provide support for an OS that is
end-of-life. From 2.13.3 on, Windows XP is no longer supported. Users are
suggested to upgrade to a newer version of Windows, or install an alternative OS
that is supported.

No attempt is made to prevent installing or running the software on Windows XP,
you can still do so at your own risk, but do not expect it to work: do not
report issues about Windows XP to the issue tracker.

From 2.13.3 onwards OS X 10.7 is no longer supported. 2.13.3 now requires 10.8+, and will communicate that to 10.7 users, rather than crashing unexpectedly.

Notable changes
===============

Fix buffer overflow in bundled upnp
------------------------------------

Bundled miniupnpc was updated to 1.9.20151008. This fixes a buffer overflow in
the XML parser during initial network discovery.

Details can be found here: http://talosintel.com/reports/TALOS-2015-0035/

This applies to the distributed executables only, not when building from source or
using distribution provided packages.

Additionally, upnp has been disabled by default. This may result in a lower
number of reachable nodes on IPv4, however this prevents future libupnpc
vulnerabilities from being a structural risk to the network
(see https://github.com/bitcoin/bitcoin/pull/6795).

Test for LowS signatures before relaying
-----------------------------------------

Make the node require the canonical 'low-s' encoding for ECDSA signatures when
relaying or mining.  This removes a nuisance malleability vector.

Consensus behavior is unchanged.

If widely deployed this change would eliminate the last remaining known vector
for nuisance malleability on SIGHASH_ALL P2PKH transactions. On the down-side
it will block most transactions made by sufficiently out of date software.

Unlike the other avenues to change txids on transactions this
one was randomly violated by all deployed groestlcoin software prior to
its discovery. So, while other malleability vectors where made
non-standard as soon as they were discovered, this one has remained
permitted. Even BIP62 did not propose applying this rule to
old version transactions, but conforming implementations have become
much more common since BIP62 was initially written.

This does not replace the need for BIP62 or similar, as miners can
still cooperate to break transactions.  Nor does it replace the
need for wallet software to handle malleability sanely[1]. This
only eliminates the cheap and irritating DOS attack.

BIP65 soft fork to enforce OP_CHECKLOCKTIMEVERIFY opcode
--------------------------------------------------------

This release includes several changes related to the [BIP65][] soft fork
which redefines the existing OP_NOP2 opcode as OP_CHECKLOCKTIMEVERIFY
(CLTV) so that a transaction output can be made unspendable until a
specified point in the future.

1. This release will only relay and mine transactions spending a CLTV
   output if they comply with the BIP65 rules as provided in code.

2. This release will produce version 4 blocks by default. Please see the
   *notice to miners* below.

3. Once 951 out of a sequence of 1,001 blocks on the local node's best block
   chain contain version 4 (or higher) blocks, this release will no
   longer accept new version 3 blocks and it will only accept version 4
   blocks if they comply with the BIP65 rules for CLTV.

For more information about the soft-forking change, please see
<https://github.com/bitcoin/bitcoin/pull/6351>

Graphs showing the progress towards block version 4 adoption may be
found at the URLs below:

- Block versions over the last 50,000 blocks as progress towards BIP65
  consensus enforcement: <http://bitcoin.sipa.be/ver-50k.png>

- Block versions over the last 2,000 blocks showing the days to the
  earliest possible BIP65 consensus-enforced block: <http://bitcoin.sipa.be/ver-2k.png>

**Notice to miners:** Bitcoin Core’s block templates are now for
version 4 blocks only, and any mining software relying on its
getblocktemplate must be updated in parallel to use libblkmaker either
version 0.4.3 or any version from 0.5.2 onward.

- If you are solo mining, this will affect you the moment you upgrade
  Bitcoin Core, which must be done prior to BIP65 achieving its 951/1001
  status.

- If you are mining with the stratum mining protocol: this does not
  affect you.

- If you are mining with the getblocktemplate protocol to a pool: this
  will affect you at the pool operator’s discretion, which must be no
  later than BIP65 achieving its 951/1001 status.

[BIP65]: https://github.com/bitcoin/bips/blob/master/bip-0065.mediawiki

BIP113 mempool-only locktime enforcement using GetMedianTimePast()
----------------------------------------------------------------

Groestlcoin transactions currently may specify a locktime indicating when
they may be added to a valid block.  Current consensus rules require
that blocks have a block header time greater than the locktime specified
in any transaction in that block.

Miners get to choose what time they use for their header time, with the
consensus rule being that no node will accept a block whose time is more
than two hours in the future.  This creates a incentive for miners to
set their header times to future values in order to include locktimed
transactions which weren't supposed to be included for up to two more
hours.

The consensus rules also specify that valid blocks may have a header
time greater than that of the median of the 11 previous blocks.  This
GetMedianTimePast() time has a key feature we generally associate with
time: it can't go backwards.

[BIP113][] specifies a soft fork that weakens this perverse incentive 
for individual miners to use a future time by requiring that valid 
blocks have a computed GetMedianTimePast() greater than the locktime 
specified in any transaction in that block.

Mempool inclusion rules currently require transactions to be valid for
immediate inclusion in a block in order to be accepted into the mempool.
This release begins applying the BIP113 rule to received transactions,
so transaction whose time is greater than the GetMedianTimePast() will
no longer be accepted into the mempool.

**Implication for miners:** you will begin rejecting transactions that
would not be valid under BIP113, which will prevent you from producing
invalid blocks if/when BIP113 is enforced on the network. Any
transactions which are valid under the current rules but not yet valid
under the BIP113 rules will either be mined by other miners or delayed
until they are valid under BIP113. Note, however, that time-based
locktime transactions are more or less unseen on the network currently.

**Implication for users:** GetMedianTimePast() always trails behind the
current time, so a transaction locktime set to the present time will be
rejected by nodes running this release until the median time moves
forward. To compensate, subtract one hour (3,600 seconds) from your
locktimes to allow those transactions to be included in mempools at
approximately the expected time.

[BIP113]: https://github.com/bitcoin/bips/blob/master/bip-0113.mediawiki

Windows bug fix for corrupted UTXO database on unclean shutdowns
----------------------------------------------------------------

Several Windows users reported that they often need to reindex the
entire blockchain after an unclean shutdown of Groestlcoin Core on Windows
(or an unclean shutdown of Windows itself). Although unclean shutdowns
remain unsafe, this release no longer relies on memory-mapped files for
the UTXO database, which significantly reduced the frequency of unclean
shutdowns leading to required reindexes during testing.

Change to wallet handling of mempool rejection
-----------------------------------------------

When a newly created transaction failed to enter the mempool due to
the limits on chains of unconfirmed transactions the sending RPC
calls would return an error.  The transaction would still be queued
in the wallet and, once some of the parent transactions were
confirmed, broadcast after the software was restarted.

This behavior has been changed to return success and to reattempt
mempool insertion at the same time transaction rebroadcast is
attempted, avoiding a need for a restart.

Transactions in the wallet which cannot be accepted into the mempool
can be abandoned with the previously existing abandontransaction RPC
(or in the GUI via a context menu on the transaction).
