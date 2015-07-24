#include "groestlcoin.h"

#inclide <sphlib/sph_groestl.h>


namespace XCoin {

uint256 HashGroestl(const ConstBuf& cbuf) {
    sph_groestl512_context  ctx_gr[2];
    static unsigned char pblank[1];
    uint512 hash[2];
	
    sph_groestl512_init(&ctx_gr[0]);
    sph_groestl512 (&ctx_gr[0], (vch.empty() ? pblank : cbuf.P), cbuf.Size);
    sph_groestl512_close(&ctx_gr[0], static_cast<void*>(&hash[0]));
	
	sph_groestl512_init(&ctx_gr[1]);
	sph_groestl512(&ctx_gr[1],static_cast<const void*>(&hash[0]), 64);
	sph_groestl512_close(&ctx_gr[1],static_cast<void*>(&hash[1]));
	
    return hash[1].trim256();
}


} // XCoin::


static const int64 nGenesisBlockRewardCoin = 1 * COIN;
int64 minimumSubsidy = 5.0 * COIN;
static const int64 nPremine = 240640 * COIN;

int64 static GetBlockSubsidy(int nHeight){
    
	
	if (nHeight == 0)
    {
        return nGenesisBlockRewardCoin;
    }
	
	if (nHeight == 1)
    {
        return nPremine;
		/*
		optimized standalone cpu miner 	60*512=30720
		standalone gpu miner 			120*512=61440
		first pool			 			70*512 =35840
		block-explorer		 			60*512 =30720
		mac wallet binary    			30*512 =15360
		linux wallet binary  			30*512 =15360
		web-site						100*512	=51200
		total									=240640
		*/
    }
    
	int64 nSubsidy = 512 * COIN;
    
    // Subsidy is reduced by 6% every 10080 blocks, which will occur approximately every 1 week
    int exponent=(nHeight / 10080);
    for(int i=0;i<exponent;i++){
        nSubsidy=nSubsidy*47;
		nSubsidy=nSubsidy/50;
    }
    if(nSubsidy<minimumSubsidy){nSubsidy=minimumSubsidy;}
    return nSubsidy;
}

int64 static GetBlockSubsidy120000(int nHeight)
{
	// Subsidy is reduced by 10% every day (1440 blocks)
	int64 nSubsidy = 250 * COIN;
	int exponent = ((nHeight - 120000) / 1440);
	for(int i=0; i<exponent; i++)
		nSubsidy = (nSubsidy * 45) / 50;

	return nSubsidy;
}

int64 static GetBlockSubsidy150000(int nHeight)
{
	// Subsidy is reduced by 1% every week (10080 blocks)
	int64 nSubsidy = 25 * COIN;
	int exponent = ((nHeight - 150000) / 10080);
	for(int i=0; i<exponent; i++)
		nSubsidy = (nSubsidy * 99) / 100;

	return (nSubsidy < minimumSubsidy)? minimumSubsidy : nSubsidy;
}

CAmount GetBlockSubsidy(int nHeight, const Consensus::Params& consensusParams)
{
	if(nHeight >= 150000)
		return GetBlockSubsidy150000(nHeight) + nFees;

	if(nHeight >= 120000)
		return GetBlockSubsidy120000(nHeight) + nFees;

	return GetBlockSubsidy(nHeight) + nFees;
}

//
// minimum amount of work that could possibly be required nTime after
// minimum work required was nBase
//
static const int64 nTargetTimespan = 86400; //1 day
static const int64 nTargetSpacing = 1 * 60; // groestlcoin every 60 seconds


unsigned int ComputeMinWork(unsigned int nBase, int64 nTime)
{
      // Testnet has min-difficulty blocks
    // after nTargetSpacing*2 time between blocks:
    if (fTestNet && nTime > nTargetSpacing*2)
        return bnProofOfWorkLimit.GetCompact();

    CBigNum bnResult;
    bnResult.SetCompact(nBase);
    while (nTime > 0 && bnResult < bnProofOfWorkLimit)
    {
        // Maximum 400% adjustment...
        bnResult *= 4;
        // ... in best-case exactly 4-times-normal target time
        nTime -= nTargetTimespan*4;
    }
    if (bnResult > bnProofOfWorkLimit)
        bnResult = bnProofOfWorkLimit;
    return bnResult.GetCompact();
}
//--------------------------
unsigned int static DarkGravityWave(const CBlockIndex* pindexLast, const CBlockHeader *pblock) {
    /* current difficulty formula, darkcoin - DarkGravity, written by Evan Duffield - evan@darkcoin.io */
    const CBlockIndex *BlockLastSolved = pindexLast;
    const CBlockIndex *BlockReading = pindexLast;
    const CBlockHeader *BlockCreating = pblock;
    BlockCreating = BlockCreating;
    int64 nBlockTimeAverage = 0;
    int64 nBlockTimeAveragePrev = 0;
    int64 nBlockTimeCount = 0;
    int64 nBlockTimeSum2 = 0;
    int64 nBlockTimeCount2 = 0;
    int64 LastBlockTime = 0;
    int64 PastBlocksMin = 12;
    int64 PastBlocksMax = 120;
    int64 CountBlocks = 0;
    CBigNum PastDifficultyAverage;
    CBigNum PastDifficultyAveragePrev;

    if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 || BlockLastSolved->nHeight < PastBlocksMin) { return bnProofOfWorkLimit.GetCompact(); }
        
    for (unsigned int i = 1; BlockReading && BlockReading->nHeight > 0; i++) {
        if (PastBlocksMax > 0 && i > PastBlocksMax) { break; }
        CountBlocks++;

        if(CountBlocks <= PastBlocksMin) {
            if (CountBlocks == 1) { PastDifficultyAverage.SetCompact(BlockReading->nBits); }
            else { PastDifficultyAverage = ((CBigNum().SetCompact(BlockReading->nBits) - PastDifficultyAveragePrev) / CountBlocks) + PastDifficultyAveragePrev; }
            PastDifficultyAveragePrev = PastDifficultyAverage;
        }

        if(LastBlockTime > 0){
            int64 Diff = (LastBlockTime - BlockReading->GetBlockTime());
            if(Diff < 0) Diff = 0;
            if(nBlockTimeCount <= PastBlocksMin) {
                nBlockTimeCount++;

                if (nBlockTimeCount == 1) { nBlockTimeAverage = Diff; }
                else { nBlockTimeAverage = ((Diff - nBlockTimeAveragePrev) / nBlockTimeCount) + nBlockTimeAveragePrev; }
                nBlockTimeAveragePrev = nBlockTimeAverage;
            }
            nBlockTimeCount2++;
            nBlockTimeSum2 += Diff;
        }
        LastBlockTime = BlockReading->GetBlockTime();

        if (BlockReading->pprev == NULL) { assert(BlockReading); break; }
        BlockReading = BlockReading->pprev;
    }
    
    CBigNum bnNew(PastDifficultyAverage);
    if (nBlockTimeCount != 0 && nBlockTimeCount2 != 0) {
            double SmartAverage = (((nBlockTimeAverage)*0.7)+((nBlockTimeSum2 / nBlockTimeCount2)*0.3));
            if(SmartAverage < 1) SmartAverage = 1;
            double Shift = nTargetSpacing/SmartAverage;

            int64 nActualTimespan = (CountBlocks*nTargetSpacing)/Shift;
            int64 nTargetTimespan = (CountBlocks*nTargetSpacing);
            if (nActualTimespan < nTargetTimespan/3)
                nActualTimespan = nTargetTimespan/3;
            if (nActualTimespan > nTargetTimespan*3)
                nActualTimespan = nTargetTimespan*3;

            // Retarget
            bnNew *= nActualTimespan;
            bnNew /= nTargetTimespan;
    }

    if (bnNew > bnProofOfWorkLimit){
        bnNew = bnProofOfWorkLimit;
    }
     
    return bnNew.GetCompact();
}

unsigned int static DarkGravityWave3(const CBlockIndex* pindexLast, const CBlockHeader *pblock) {
    /* current difficulty formula, darkcoin - DarkGravity v3, written by Evan Duffield - evan@darkcoin.io */
    const CBlockIndex *BlockLastSolved = pindexLast;
    const CBlockIndex *BlockReading = pindexLast;
    const CBlockHeader *BlockCreating = pblock;
    BlockCreating = BlockCreating;
    int64 nActualTimespan = 0;
    int64 LastBlockTime = 0;
    int64 PastBlocksMin = 24;
    int64 PastBlocksMax = 24;
    int64 CountBlocks = 0;
    CBigNum PastDifficultyAverage;
    CBigNum PastDifficultyAveragePrev;

    if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 || BlockLastSolved->nHeight < PastBlocksMin) { 
        return bnProofOfWorkLimit.GetCompact(); 
    }
        
    for (unsigned int i = 1; BlockReading && BlockReading->nHeight > 0; i++) {
        if (PastBlocksMax > 0 && i > PastBlocksMax) { break; }
        CountBlocks++;

        if(CountBlocks <= PastBlocksMin) {
            if (CountBlocks == 1) { PastDifficultyAverage.SetCompact(BlockReading->nBits); }
            else { PastDifficultyAverage = ((PastDifficultyAveragePrev * CountBlocks)+(CBigNum().SetCompact(BlockReading->nBits))) / (CountBlocks+1); }
            PastDifficultyAveragePrev = PastDifficultyAverage;
        }

        if(LastBlockTime > 0){
            int64 Diff = (LastBlockTime - BlockReading->GetBlockTime());
            nActualTimespan += Diff;
        }
        LastBlockTime = BlockReading->GetBlockTime();      

        if (BlockReading->pprev == NULL) { assert(BlockReading); break; }
        BlockReading = BlockReading->pprev;
    }
    
    CBigNum bnNew(PastDifficultyAverage);

    int64 nTargetTimespan = CountBlocks*nTargetSpacing;

    if (nActualTimespan < nTargetTimespan/3)
        nActualTimespan = nTargetTimespan/3;
    if (nActualTimespan > nTargetTimespan*3)
        nActualTimespan = nTargetTimespan*3;

    // Retarget
    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

    if (bnNew > bnProofOfWorkLimit){
        bnNew = bnProofOfWorkLimit;
    }
     
    return bnNew.GetCompact();
}
//----------------------

unsigned int static GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock)
{
    if (fTestNet)
    {//test net, DGWv3 since block 10
        if (pindexLast->nHeight >= (10 - 1))
            return DarkGravityWave3(pindexLast, pblock);
    }
    else
    {//main net, DGWv3 since block 100,000
        if (pindexLast->nHeight >= (100000 - 1))
            return DarkGravityWave3(pindexLast, pblock);
    }

    return DarkGravityWave(pindexLast, pblock);
}



/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.powLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        /** 
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xf9;
        pchMessageStart[1] = 0xbe;
        pchMessageStart[2] = 0xb4;
        pchMessageStart[3] = 0xd4;
        vAlertPubKey = ParseHex("04fc9702847840aaf195de8442ebecedf5b095cdbb9bc716bda9110971b28a49e0ead8564ff0db22209e0374782c093bb899692d524e9d6a6956e7c5ecbcd68284");
        nDefaultPort = 1331;
        nPruneAfterHeight = 10000000;

        /**
         * Build the genesis block. Note that the output of its generation
         * transaction cannot be spent since it did not originally exist in the
         * database.
         *
         * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
         *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
         *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
         *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
         *   vMerkleTree: 4a5e1e
         */
        const char* pszTimestamp = "Pressure must be put on Vladimir Putin over Crimea";
        CMutableTransaction txNew;
        txNew.nVersion = 1;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 50 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock.SetNull();
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 112;
        genesis.nTime    = 1395342829;
        genesis.nBits    = 0x1d00ffff;
        genesis.nNonce   = 220035;

        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"));
        assert(genesis.hashMerkleRoot == uint256S("0x4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"));

        vSeeds.push_back(CDNSSeedData("groestlcoin.net", "groestlcoin.net"));
        vSeeds.push_back(CDNSSeedData("groestlcoin.org", "groestlcoin.org"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,0);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

		checkpointData = Checkpoints::CCheckpointData{
			boost::assign::map_list_of
			(28888, uint256S("0x00000000000228ce19f55cf0c45e04c7aa5a6a873ed23902b3654c3c49884502"))
			(58888, uint256S("0x0000000000dd85f4d5471febeb174a3f3f1598ab0af6616e9f266b56272274ef"))
			(111111, uint256S("0x00000000013de206275ee83f93bee57622335e422acbf126a37020484c6e113c"))
			(669286, uint256S("0x000000001727e1d13ca7272dee43996efdfc6b3b57a6d2b0c48257ef52f40bcb")),
			1436539093, // * UNIX timestamp of last checkpoint block
			0,   // * total number of transactions between genesis and last checkpoint
						//   (the tx=... number in the SetBestChain debug.log lines)
			100.0     // * estimated number of transactions per day after checkpoint
		};
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 100;
        consensus.fPowAllowMinDifficultyBlocks = true;
        pchMessageStart[0] = 0x0b;
        pchMessageStart[1] = 0x11;
        pchMessageStart[2] = 0x09;
        pchMessageStart[3] = 0x07;
        vAlertPubKey = ParseHex("04302390343f91cc401d56d68b123028bf52e5fca1939df127f63c6467cdf9c8e2c14b61104cf817d0b780da337893ecc4aaff1309e536162dabbdb45200ca2b0a");
        nDefaultPort = 18333;
        nPruneAfterHeight = 1000;

        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1296688602;
        genesis.nNonce = 414098458;
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x000000000933ea01ad0ee984209779baaec3ced90fa3f408719526f8d77f4943"));

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("groestlcoin.net", "groestlcoin.net"));
        vSeeds.push_back(CDNSSeedData("groestlcoin.org", "groestlcoin.org"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

		checkpointData = Checkpoints::CCheckpointData {
			boost::assign::map_list_of
			(546, uint256S("000000002a936ca763904c3c35fce2f3556c559c0214345d31b1bcebf76acb70")),
			1337966069,
			1488,
			300
		};

    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xda;
        genesis.nTime = 1296688602;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 2;
        consensus.hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 18444;
        assert(consensus.hashGenesisBlock == uint256S("0x0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206"));
        nPruneAfterHeight = 1000;

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

		checkpointData = Checkpoints::CCheckpointData {
			boost::assign::map_list_of
			(0, uint256S("0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206")),
			0,
			0,
			0
		};
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams &Params(CBaseChainParams::Network network) {
    switch (network) {
        case CBaseChainParams::MAIN:
            return mainParams;
        case CBaseChainParams::TESTNET:
            return testNetParams;
        case CBaseChainParams::REGTEST:
            return regTestParams;
        default:
            assert(false && "Unimplemented network");
            return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network) {
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}


