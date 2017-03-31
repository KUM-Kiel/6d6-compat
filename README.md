# 6D6 Compatibility Tools

This is a small collection of tools to ensure compatibility of the new 6D6 Datalogger with the old Send workflows.

This is especially useful when you are in the process of migrating to the new 6D6 Datalogger but still have a lot of old recorders and wish to use all the models side by side.

## Installation

The easiest way to install the tools is to download one of the binary releases.

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
