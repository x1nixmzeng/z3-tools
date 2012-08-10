z3-tools
========

**NOTE** This project now hosts the various generic tools I write for RaiderZ and GunZ2


## Z3Ex

This is the filesystem extractor which can be built for RaiderZ or GunZ2. It is a DLL module which depends on a few client functions which have not yet been fully reversed.

At this point, the offsets are hardcoded for each client revision and a code cave is required to start the Z3Ex functions from the each client.


## launcherBinTxt

This is a tool to convert the `.bin` files (used by some publishers) to plaintext. They contain the configuration settings related to the update process.

The launcher downloads patch files using the `SYS_REMOTEPATCHFILEARG` value, and decompresses `buildver.mvf` (12 bytes) to get these values. This also happens to share the same decompression technique (and key?) as the filesystem.