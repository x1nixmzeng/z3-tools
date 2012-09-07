z3-tools
========

This project contains various generic tools I have written for games which use the RealSpace3 engine.


## launcherBinTxt

**NOTE** This tool is now deprecated, but will work with clients prior to August 30th 2012

This is a tool that can convert the `.bin` files to plaintext. They contain the configuration settings related to the update process and localised interface strings of the launcher.

    Unpack
        launcherBinTxt.exe u "RaiderZ Launcher.bin"
    Repack
        launcherBinTxt.exe r "RaiderZ Launcher.txt"


## mpfInfo

Simple tool to parse and extract the commands from update files using the `MAIET Patch File` format. The format allows the updater to manipulate sections of data and add or remove entire files.

For more information, ![click here](https://github.com/x1nixmzeng/z3-tools/blob/master/mpfInfo/README.md)


Mature Projects
===============

## Z3Ex

This project is now part of ![z3ResEx](https://github.com/x1nixmzeng/z3ResEx)

## z3ResourceExtractor-wip

This project is now part of ![z3ResEx](https://github.com/x1nixmzeng/z3ResEx)
