#!/bin/bash

sed -i "s/\bbitcoin\b/shard/g" *.ts
sed -i "s/\bBitcoin\b/Shard/g" *.ts
sed -i "s/\bBITCOINS\b/SHARDS/g" *.ts
sed -i "s/\bbitcoins\b/shards/g" *.ts
sed -i "s/\bBitcoins/Shards/g" *.ts

sed -i "s/ bitcoin/ shard/g" *.ts
sed -i "s/ Bitcoin/ Shard/g" *.ts
sed -i "s/ BITCOIN/ SHARD/g" *.ts
