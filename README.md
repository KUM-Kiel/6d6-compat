# 6D6 Compatibility Tools

This is a small collection of tools to convert data from the new 6D6 datalogger to other formats.

The main supported format is the MiniSEED format.
It is widely accepted in the seismological community and ensures maximum compatibility with different workflows.

For compatibility with SEND software, the s2x format can be used.
This is especially useful when you are in the process of migrating to the new 6D6 Datalogger but still have a lot of old recorders and wish to use all the models side by side.

As a planned feature, the implementation of the SEG-Y format is in progress.

## Installation

The easiest way to install the tools is to [download one of the binary releases](https://github.com/KUM-Kiel/6d6-compat/releases/latest).

After extraction the programs can be installed by typing

```text
$ sudo make install
```

## Building from source

To compile the programs from source you need to have installed a C compiler, Ruby and the rake-c gem.

```text
$ sudo apt-get install build-essential gcc-multilib git ruby
$ sudo gem install rake-c
$ git clone https://github.com/KUM-Kiel/6d6-compat.git
$ cd 6d6-compat
$ rake
$ rake install
```

## Licence

The program is published under the terms of the GNU GPL 3.0. See the `LICENCE` file.

Some parts of the program incorporate public domain or BSD licensed code.
