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

Database cache memory increased
--------------------------------

As a result of growth of the UTXO set, performance with the prior default
database cache of 100 MiB has suffered.
For this reason the default was changed to 300 MiB in this release.

For nodes on low-memory systems, the database cache can be changed back to
100 MiB (or to another value) by either:

- Adding `dbcache=100` in bitcoin.conf
- Changing it in the GUI under `Options → Size of database cache`

Note that the database cache setting has the most performance impact
during initial sync of a node, and when catching up after downtime.


bitcoin-cli: arguments privacy
------------------------------

The RPC command line client gained a new argument, `-stdin`
to read extra arguments from standard input, one per line until EOF/Ctrl-D.
For example:

    $ src/bitcoin-cli -stdin walletpassphrase
    mysecretcode
    120
    ..... press Ctrl-D here to end input
    $

It is recommended to use this for sensitive information such as wallet
passphrases, as command-line arguments can usually be read from the process
table by any user on the system.

C++11 and Python 3
------------------

Various code modernizations have been done. The Bitcoin Core code base has
started using C++11. This means that a C++11-capable compiler is now needed for
building. Effectively this means GCC 4.7 or higher, or Clang 3.3 or higher.

When cross-compiling for a target that doesn't have C++11 libraries, configure with
`./configure --enable-glibc-back-compat ... LDFLAGS=-static-libstdc++`.

For running the functional tests in `qa/rpc-tests`, Python3.4 or higher is now
required.

Compact Block support (BIP 152)
-------------------------------

Support for block relay using the Compact Blocks protocol has been implemented
in PR 8068.

The primary goal is reducing the bandwidth spikes at relay time, though in many
cases it also reduces propagation delay. It is automatically enabled between
compatible peers.
[BIP 152](https://github.com/bitcoin/bips/blob/master/bip-0152.mediawiki)

As a side-effect, ordinary non-mining nodes will download and upload blocks
faster if those blocks were produced by miners using similar transaction
filtering policies. This means that a miner who produces a block with many
transactions discouraged by your node will be relayed slower than one with
only transactions already in your memory pool. The overall effect of such
relay differences on the network may result in blocks which include widely-
discouraged transactions losing a stale block race, and therefore miners may
wish to configure their node to take common relay policies into consideration.

Hierarchical Deterministic Key Generation
-----------------------------------------
Newly created wallets will use hierarchical deterministic key generation
according to BIP32 (keypath m/0'/0'/k').
Existing wallets will still use traditional key generation.

Backups of HD wallets, regardless of when they have been created, can
therefore be used to re-generate all possible private keys, even the
ones which haven't already been generated during the time of the backup.
**Attention:** Encrypting the wallet will create a new seed which requires
a new backup!

Wallet dumps (created using the `dumpwallet` RPC) will contain the deterministic
seed. This is expected to allow future versions to import the seed and all
associated funds, but this is not yet implemented.

HD key generation for new wallets can be disabled by `-usehd=0`. Keep in
mind that this flag only has affect on newly created wallets.
You can't disable HD key generation once you have created a HD wallet.

There is no distinction between internal (change) and external keys.

HD wallets are incompatible with older versions of Bitcoin Core.

[Pull request](https://github.com/bitcoin/bitcoin/pull/8035/files), [BIP 32](https://github.com/bitcoin/bips/blob/master/bip-0032.mediawiki)

Mining transaction selection ("Child Pays For Parent")
------------------------------------------------------

The mining transaction selection algorithm has been replaced with an algorithm
that selects transactions based on their feerate inclusive of unconfirmed
ancestor transactions.  This means that a low-fee transaction can become more
likely to be selected if a high-fee transaction that spends its outputs is
relayed.

With this change, the `-blockminsize` command line option has been removed.

The command line option `-blockmaxsize` remains an option to specify the
maximum number of serialized bytes in a generated block.  In addition, the new
command line option `-blockmaxweight` has been added, which specifies the
maximum "block weight" of a generated block, as defined by [BIP 141 (Segregated
Witness)] (https://github.com/bitcoin/bips/blob/master/bip-0141.mediawiki).

In preparation for Segregated Witness, the mining algorithm has been modified
to optimize transaction selection for a given block weight, rather than a given
number of serialized bytes in a block.  In this release, transaction selection
is unaffected by this distinction (as BIP 141 activation is not supported on
mainnet in this release, see above), but in future releases and after BIP 141
activation, these calculations would be expected to differ.

For optimal runtime performance, miners using this release should specify
`-blockmaxweight` on the command line, and not specify `-blockmaxsize`.
Additionally (or only) specifying `-blockmaxsize`, or relying on default
settings for both, may result in performance degradation, as the logic to
support `-blockmaxsize` performs additional computation to ensure that
constraint is met.  (Note that for mainnet, in this release, the equivalent
parameter for `-blockmaxweight` would be four times the desired
`-blockmaxsize`.  See [BIP 141]
(https://github.com/bitcoin/bips/blob/master/bip-0141.mediawiki) for additional
details.)

In the future, the `-blockmaxsize` option may be removed, as block creation is
no longer optimized for this metric.  Feedback is requested on whether to
deprecate or keep this command line option in future releases.

Reindexing changes
------------------

In earlier versions, reindexing did validation while reading through the block
files on disk. These two have now been split up, so that all blocks are known
before validation starts. This was necessary to make certain optimizations that
are available during normal synchronizations also available during reindexing.

The two phases are distinct in the Bitcoin-Qt GUI. During the first one,
"Reindexing blocks on disk" is shown. During the second (slower) one,
"Processing blocks on disk" is shown.

It is possible to only redo validation now, without rebuilding the block index,
using the command line option `-reindex-chainstate` (in addition to
`-reindex` which does both). This new option is useful when the blocks on disk
are assumed to be fine, but the chainstate is still corrupted. It is also
useful for benchmarks.

Removal of internal miner
--------------------------

As CPU mining has been useless for a long time, the internal miner has been
removed in this release, and replaced with a simpler implementation for the
test framework.

The overall result of this is that `setgenerate` RPC call has been removed, as
well as the `-gen` and `-genproclimit` command-line options.

For testing, the `generate` call can still be used to mine a block, and a new
RPC call `generatetoaddress` has been added to mine to a specific address. This
works with wallet disabled.


New bytespersigop implementation
--------------------------------

The former implementation of the bytespersigop filter accidentally broke bare
multisig (which is meant to be controlled by the `permitbaremultisig` option),
since the consensus protocol always counts these older transaction forms as 20
sigops for backwards compatibility. Simply fixing this bug by counting more
accurately would have reintroduced a vulnerability. It has therefore been
replaced with a new implementation that rather than filter such transactions,
instead treats them (for fee purposes only) as if they were in fact the size
of a transaction actually using all 20 sigops.

Low-level P2P changes
----------------------

- The optional new p2p message "feefilter" is implemented and the protocol
  version is bumped to 70013. Upon receiving a feefilter message from a peer,
  a node will not send invs for any transactions which do not meet the filter
  feerate. [BIP 133](https://github.com/bitcoin/bips/blob/master/bip-0133.mediawiki)

- The P2P alert system has been removed in PR #7692 and the `alert` P2P message
  is no longer supported.

- The transaction relay mechanism used to relay one quarter of all transactions
  instantly, while queueing up the rest and sending them out in batch. As
  this resulted in chains of dependent transactions being reordered, it
  systematically hurt transaction relay. The relay code was redesigned in PRs
  \#7840 and #8082, and now always batches transactions announcements while also
  sorting them according to dependency order. This significantly reduces orphan
  transactions. To compensate for the removal of instant relay, the frequency of
  batch sending was doubled for outgoing peers.

- Since PR #7840 the BIP35 `mempool` command is also subject to batch processing.
  Also the `mempool` message is no longer handled for non-whitelisted peers when
  `NODE_BLOOM` is disabled through `-peerbloomfilters=0`.

- The maximum size of orphan transactions that are kept in memory until their
  ancestors arrive has been raised in PR #8179 from 5000 to 99999 bytes. They
  are now also removed from memory when they are included in a block, conflict
  with a block, and time out after 20 minutes.

- We respond at most once to a getaddr request during the lifetime of a
  connection since PR #7856.

- Connections to peers who have recently been the first one to give us a valid
  new block or transaction are protected from disconnections since PR #8084.

Low-level RPC changes
----------------------

- RPC calls have been added to output detailed statistics for individual mempool
  entries, as well as to calculate the in-mempool ancestors or descendants of a
  transaction: see `getmempoolentry`, `getmempoolancestors`, `getmempooldescendants`.

- `gettxoutsetinfo` UTXO hash (`hash_serialized`) has changed. There was a divergence between
  32-bit and 64-bit platforms, and the txids were missing in the hashed data. This has been
  fixed, but this means that the output will be different than from previous versions.

- Full UTF-8 support in the RPC API. Non-ASCII characters in, for example,
  wallet labels have always been malformed because they weren't taken into account
  properly in JSON RPC processing. This is no longer the case. This also affects
  the GUI debug console.

- Asm script outputs replacements for OP_NOP2 and OP_NOP3

  - OP_NOP2 has been renamed to OP_CHECKLOCKTIMEVERIFY by [BIP 
65](https://github.com/bitcoin/bips/blob/master/bip-0065.mediawiki)

  - OP_NOP3 has been renamed to OP_CHECKSEQUENCEVERIFY by [BIP 
112](https://github.com/bitcoin/bips/blob/master/bip-0112.mediawiki)

  - The following outputs are affected by this change:

    - RPC `getrawtransaction` (in verbose mode)
    - RPC `decoderawtransaction`
    - RPC `decodescript`
    - REST `/rest/tx/` (JSON format)
    - REST `/rest/block/` (JSON format when including extended tx details)
    - `bitcoin-tx -json`

- The sorting of the output of the `getrawmempool` output has changed.

- New RPC commands: `generatetoaddress`, `importprunedfunds`, `removeprunedfunds`, `signmessagewithprivkey`,
  `getmempoolancestors`, `getmempooldescendants`, `getmempoolentry`,
  `createwitnessaddress`, `addwitnessaddress`.

- Removed RPC commands: `setgenerate`, `getgenerate`.

- New options were added to `fundrawtransaction`: `includeWatching`, `changeAddress`, `changePosition` and `feeRate`.

Low-level ZMQ changes
----------------------

- Each ZMQ notification now contains an up-counting sequence number that allows
  listeners to detect lost notifications.
  The sequence number is always the last element in a multi-part ZMQ notification and
  therefore backward compatible. Each message type has its own counter.
  PR [#7762](https://github.com/bitcoin/bitcoin/pull/7762).

Segregated witness soft fork
----------------------------

Segregated witness (segwit) is a soft fork that, if activated, will
allow transaction-producing software to separate (segregate) transaction
signatures (witnesses) from the part of the data in a transaction that is
covered by the txid. This provides several immediate benefits:

- **Elimination of unwanted transaction malleability:** Segregating the witness
  allows both existing and upgraded software to calculate the transaction
  identifier (txid) of transactions without referencing the witness, which can
  sometimes be changed by third-parties (such as miners) or by co-signers in a
  multisig spend. This solves all known cases of unwanted transaction
  malleability, which is a problem that makes programming Bitcoin wallet
  software more difficult and which seriously complicates the design of smart
  contracts for Bitcoin.

- **Capacity increase:** Segwit transactions contain new fields that are not
  part of the data currently used to calculate the size of a block, which
  allows a block containing segwit transactions to hold more data than allowed
  by the current maximum block size. Estimates based on the transactions
  currently found in blocks indicate that if all wallets switch to using
  segwit, the network will be able to support about 70% more transactions. The
  network will also be able to support more of the advanced-style payments
  (such as multisig) than it can support now because of the different weighting
  given to different parts of a transaction after segwit activates (see the
  following section for details).

- **Weighting data based on how it affects node performance:** Some parts of
  each Bitcoin block need to be stored by nodes in order to validate future
  blocks; other parts of a block can be immediately forgotten (pruned) or used
  only for helping other nodes sync their copy of the block chain.  One large
  part of the immediately prunable data are transaction signatures (witnesses),
  and segwit makes it possible to give a different "weight" to segregated
  witnesses to correspond with the lower demands they place on node resources.
  Specifically, each byte of a segregated witness is given a weight of 1, each
  other byte in a block is given a weight of 4, and the maximum allowed weight
  of a block is 4 million.  Weighting the data this way better aligns the most
  profitable strategy for creating blocks with the long-term costs of block
  validation.

- **Signature covers value:** A simple improvement in the way signatures are
  generated in segwit simplifies the design of secure signature generators
  (such as hardware wallets), reduces the amount of data the signature
  generator needs to download, and allows the signature generator to operate
  more quickly.  This is made possible by having the generator sign the amount
  of bitcoins they think they are spending, and by having full nodes refuse to
  accept those signatures unless the amount of bitcoins being spent is exactly
  the same as was signed.  For non-segwit transactions, wallets instead had to
  download the complete previous transactions being spent for every payment
  they made, which could be a slow operation on hardware wallets and in other
  situations where bandwidth or computation speed was constrained.

- **Linear scaling of sighash operations:** In 2015 a block was produced that
  required about 25 seconds to validate on modern hardware because of the way
  transaction signature hashes are performed.  Other similar blocks, or blocks
  that could take even longer to validate, can still be produced today.  The
  problem that caused this can't be fixed in a soft fork without unwanted
  side-effects, but transactions that opt-in to using segwit will now use a
  different signature method that doesn't suffer from this problem and doesn't
  have any unwanted side-effects.

- **Increased security for multisig:** Bitcoin addresses (both P2PKH addresses
  that start with a '1' and P2SH addresses that start with a '3') use a hash
  function known as RIPEMD-160.  For P2PKH addresses, this provides about 160
  bits of security---which is beyond what cryptographers believe can be broken
  today.  But because P2SH is more flexible, only about 80 bits of security is
  provided per address. Although 80 bits is very strong security, it is within
  the realm of possibility that it can be broken by a powerful adversary.
  Segwit allows advanced transactions to use the SHA256 hash function instead,
  which provides about 128 bits of security  (that is 281 trillion times as
  much security as 80 bits and is equivalent to the maximum bits of security
  believed to be provided by Bitcoin's choice of parameters for its Elliptic
  Curve Digital Security Algorithm [ECDSA].)

- **More efficient almost-full-node security** Satoshi Nakamoto's original
  Bitcoin paper describes a method for allowing newly-started full nodes to
  skip downloading and validating some data from historic blocks that are
  protected by large amounts of proof of work.  Unfortunately, Nakamoto's
  method can't guarantee that a newly-started node using this method will
  produce an accurate copy of Bitcoin's current ledger (called the UTXO set),
  making the node vulnerable to falling out of consensus with other nodes.
  Although the problems with Nakamoto's method can't be fixed in a soft fork,
  Segwit accomplishes something similar to his original proposal: it makes it
  possible for a node to optionally skip downloading some blockchain data
  (specifically, the segregated witnesses) while still ensuring that the node
  can build an accurate copy of the UTXO set for the block chain with the most
  proof of work.  Segwit enables this capability at the consensus layer, but
  note that Bitcoin Core does not provide an option to use this capability as
  of this 0.13.1 release.

- **Script versioning:** Segwit makes it easy for future soft forks to allow
  Bitcoin users to individually opt-in to almost any change in the Bitcoin
  Script language when those users receive new transactions.  Features
  currently being researched by Bitcoin Core contributors that may use this
  capability include support for Schnorr signatures, which can improve the
  privacy and efficiency of multisig transactions (or transactions with
  multiple inputs), and Merklized Abstract Syntax Trees (MAST), which can
  improve the privacy and efficiency of scripts with two or more conditions.
  Other Bitcoin community members are studying several other improvements
  that can be made using script versioning.

Activation for the segwit soft fork is being managed using BIP9
versionbits.  Segwit's version bit is bit 1, and nodes will begin
tracking which blocks signal support for segwit at the beginning of the
first retarget period after segwit's start date of 15 November 2016.  If
95% of blocks within a 2,016-block retarget period (about two weeks)
signal support for segwit, the soft fork will be locked in.  After
another 2,016 blocks, segwit will activate.

For more information about segwit, please see the [segwit FAQ][], the
[segwit wallet developers guide][] or BIPs [141][BIP141], [143][BIP143],
[144][BIP144], and [145][BIP145].  If you're a miner or mining pool
operator, please see the [versionbits FAQ][] for information about
signaling support for a soft fork.

[Segwit FAQ]: https://bitcoincore.org/en/2016/01/26/segwit-benefits/
[segwit wallet developers guide]: https://bitcoincore.org/en/segwit_wallet_dev/
[BIP141]: https://github.com/bitcoin/bips/blob/master/bip-0141.mediawiki
[BIP143]: https://github.com/bitcoin/bips/blob/master/bip-0143.mediawiki
[BIP144]: https://github.com/bitcoin/bips/blob/master/bip-0144.mediawiki
[BIP145]: https://github.com/bitcoin/bips/blob/master/bip-0145.mediawiki
[versionbits FAQ]: https://bitcoincore.org/en/2016/06/08/version-bits-miners-faq/

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
