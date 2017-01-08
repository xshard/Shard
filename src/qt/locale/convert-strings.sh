#!/bin/bash

sed -i "s/\bbitcoin\b/groestlcoin/g" *.ts
sed -i "s/\bBitcoin\b/Groestlcoin/g" *.ts
sed -i "s/\bBITCOINS\b/GROESTLCOINS/g" *.ts
sed -i "s/\bbitcoins\b/groestlcoins/g" *.ts
sed -i "s/\bBitcoins/Groestlcoins/g" *.ts

sed -i "s/ bitcoin/ groestlcoin/g" *.ts
sed -i "s/ Bitcoin/ Groestlcoin/g" *.ts
sed -i "s/ BITCOIN/ GROESTLCOIN/g" *.ts
