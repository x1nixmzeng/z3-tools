z3-tools
========

This project contains various generic tools I have written for games which use the RealSpace3 engine.


## Z3Ex

This is the filesystem extractor which can be built for RaiderZ or GunZ2. It is a DLL module which depends on a few client functions which have not yet been fully reversed (see Future Projects).

At this point, the offsets are hardcoded for each client revision and a code cave is required to start the Z3Ex functions.


## launcherBinTxt

**NOTE** This tool is now deprecated, but will work with clients prior to August 30th 2012

This is a tool to convert the `.bin` files (not used by the Korean publisher) to plaintext. They contain the configuration settings related to the update process and localised interface strings of the launcher.


## mpfInfo

Simple tool to parse and extract the commands from update files using the `MAIET Patch File` format. The format allows the updater to manipulate sections of data and add or remove entire files.


## z3ResourceExtractor-wip

An unfinished work-in-progress project reflecting my current efforts in reversing the decompression function used by the client to unpack the data (this is the `Uncompress()` function from Z3Ex).


Future Projects
===============

## z3ResourceExtractor

This will evolve from my various reverse engineering efforts and contributions made from members of XeNTaX, and will replace Z3Ex.

