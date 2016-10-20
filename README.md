# 6D6 Compatibility Tools

This is a small collection of tools to ensure compatibility of the new 6D6 Datalogger with the old Send workflows.

This is especially useful when you are in the process of migrating to the new 6D6 Datalogger but still have a lot of old recorders and wish to use all the models side by side.

## 6d6copy

Copies data from an SD card into a 6D6 file.

When the `-p` option is given, the progress will be reported.

### Usage

```text
$ 6d6copy [-p|--progress] /dev/sdX > out.6d6
```

## 6d6read

Converts a 6D6 file to Send2x format.

When the `-p` option is given, the progress will be reported.

### Usage

```text
$ 6d6read [-p|--progress] < in.6d6 > out.s2x
```

## s2xshift
