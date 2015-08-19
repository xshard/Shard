
Debian
====================
This directory contains files used to package groestlcoind/groestlcoin-qt
for Debian-based Linux systems. If you compile groestlcoind/groestlcoin-qt yourself, there are some useful files here.

## groestlcoin: URI support ##


groestlcoin-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install groestlcoin-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your groestlcoin-qt binary to `/usr/bin`
and the `../../share/pixmaps/groestlcoin128.png` to `/usr/share/pixmaps`

groestlcoin-qt.protocol (KDE)

