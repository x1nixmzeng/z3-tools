Z3Ex.DLL
========

Z3Ex is a RealSpace3 filesystem extractor which can be built for RaiderZ or GunZ2.

There are a few client functions which have not been fully reversed which need to be called in order to unpack the files.

At this stage, the offsets are hardcoded for each client revision and a code cave is required to start the Z3Ex functions from the each client.

## Bonus

I have uploaded `rzlauncherBin`, a program to convert `RaiderZ Launcher.bin` to plaintext. This is part of the update process, where the launcher then decompresses `buildver.mvf` (12-bytes) to determine which patch versions to use.
