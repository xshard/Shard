#!/bin/bash

sed -i "s/\bbitcoin\b/groestlcoin/g" *.ts
sed -i "s/\bBitcoin\b/Groestlcoin/g" *.ts
sed -i "s/\bBITCOINS\b/GROESTLCOINS/g" *.ts
sed -i "s/\bbitcoins\b/groestlcoins/g" *.ts
