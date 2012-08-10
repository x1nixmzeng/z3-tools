z3-tools
========

**NOTE** This project now hosts the various tools I write for RaiderZ and GunZ2 (which use the RealSpace3)

## Z3Ex

This is the filesystem extraction which can be build for RaiderZ or GunZ2.

There are a few client functions which have not been fully reversed which need to be called in order to unpack the files.

At this point, the offsets are hardcoded for each client revision and a code cave is required to start the Z3Ex functions from the each client.

## launcherBinTxt

This is a tool to convert the `.bin` files (used by some publishers) to plaintext. They contain the configuration settings related to the update process.

The launcher downloads patch files using the `SYS_REMOTEPATCHFILEARG` value, and decompresses `buildver.mvf` (12 bytes) to get these values. This also happens to share the same decompression technique (and key?) as the filesystem.