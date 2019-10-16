# Rucio `FUSE-posix` interface

This repository carries the best attempt at making the Rucio namespace `posix`-mountable.

It is based on `C` code (for the custom `FUSE` specialization) and on `C++11` for utilities and wrappers.

This tool is intended as an **alpha** version unless explicitly said and has to be considered WIP.

Please note that the first target is to get a **read-only file system**.

## Access pattern
The Rucio file catalog is much flatter than that of a usual `POSIX` filesystems and as such its representation has to be structured according to that:

- the root of the mount is intended to be a `cvmfs`-like "`/rucio`";
- the first level of the tree (namely `/rucio/*`) should be filled with all the available scopes;
- each scope should appear as a directory filled with its DiDs;
- file DiDs will appear as files;
- container and dataset DiDs will appear as directories;
- datasets and containers might include already represented DiDs: a routine to handle such loops should be present.

## TODOs
- implement `getattr`, `readdir` and `read` methods
- write tests of the code written up to now
- surely a lot more...

## How to build
To build the software please run:

```[shell]
/path/to/cmake --build /build --target rucio-fuse -- -j 4
```

To complete the build `libcurl` and `libfuse` must be present:
`cmake` will try to locate them for you and trigger some build messages if unable to do so.
