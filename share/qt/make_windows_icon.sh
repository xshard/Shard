#!/bin/bash
# create multiresolution windows icon
ICON_SRC=../../src/qt/res/icons/groestlcoin.png
ICON_DST=../../src/qt/res/icons/groestlcoin.ico
convert ${ICON_SRC} -resize 16x16 groestlcoin-16.png
convert ${ICON_SRC} -resize 32x32 groestlcoin-32.png
convert ${ICON_SRC} -resize 48x48 groestlcoin-48.png
convert groestlcoin-16.png groestlcoin-32.png groestlcoin-48.png ${ICON_DST}

