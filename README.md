z3-tools
========

This project contains various generic tools I have written for games which use the RealSpace3 engine.


## Z3Ex

This is the filesystem extractor which can be built for RaiderZ or GunZ2. It is a DLL module which depends on a few client functions which have not yet been fully reversed (see Future Projects).

At this point, the offsets are hardcoded for each client revision and a code cave is required to start the Z3Ex functions.


## launcherBinTxt

**NOTE** This tool will not work with GunZ2 CBT (August 30th 2012)


This is a tool to convert the `.bin` files (not used by the Korean publisher) to plaintext. They contain the configuration settings related to the update process.

The launcher downloads patch files using a combination of the `SYS_REMOTEPATCHFILEARG` value and the version information from `buildver.mvf`. This file also happens to share the same decompression algorithm as the filesystem.


**NOTE** An unreleased build of GunZ2 used a JSON patch format:

    {
       "Version" : {
          "ClientExe" : 4,
          "Launcher" : 1
       }
    }


## mpfInfo

Simple tool to parse and extract the commands from update files using the `MAIET Patch File` format. The format allows the updater to manipulate sections of data and add or remove entire files.


Future Projects
===============


