z3-tools
========

**NOTE** This project now hosts the various generic tools I write for RaiderZ and GunZ2


## Z3Ex

This is the filesystem extractor which can be built for RaiderZ or GunZ2. It is a DLL module which depends on a few client functions which have not yet been fully reversed (see Future Projects).

At this point, the offsets are hardcoded for each client revision and a code cave is required to start the Z3Ex functions.


## launcherBinTxt

This is a tool to convert the `.bin` files (not used by the Korean publisher) to plaintext. They contain the configuration settings related to the update process.

The launcher downloads patch files using a combination of the `SYS_REMOTEPATCHFILEARG` value and the version information from `buildver.mvf` (12 bytes). This file also happens to share the same decompression technique (and key?) as the filesystem.


**NOTE** An unreleased build of GunZ2 used a JSON patch format:

    {
       "Version" : {
          "ClientExe" : 4,
          "Launcher" : 1
       }
    }


## mpfInfo

Very simple tool to parse update files using the `MAIET Patch File` format. The format allows for an updater to modify specific bytes of filedata (ontop of adding and removing entire files).


Future Projects
===============

The version information is needed before the patch files can be downloaded and applied: this is obtained by decrypting `buildver.mvf`, which is the current goal.
