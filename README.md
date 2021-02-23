# cmd-histogram

Tool to make histogram with CERN ROOT.

## Requirement

- CERN ROOT https://root.cern.ch

## Install

Need to change install directory via src/Makefile if you needed to install some directory.

```
$ cd src && make
```

It generates the binary into ./bin.

## Usage

This analyze sdtin. You need to pipe number separated by newline into this.
In default, it overwrite out.root in current directory.

For example:

```
$ ls -l | awk '{print $5}' | ./bin/cmdhistogram --xmin 0. --xmax 1e6 --nbins 100 -o filesize.root
```

Copyright 2021 Shota Izumiyama

