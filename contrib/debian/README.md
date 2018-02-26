
Debian
====================
This directory contains files used to package shardd/shard-qt
for Debian-based Linux systems. If you compile shardd/shard-qt yourself, there are some useful files here.

## shard: URI support ##


shard-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install shard-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your shard-qt binary to `/usr/bin`
and the `../../share/pixmaps/shard128.png` to `/usr/share/pixmaps`

shard-qt.protocol (KDE)

