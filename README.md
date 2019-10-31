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

## DONEs
- Curl-REST wrapper based on `libcurl` has been implemented in the form of `GET` and `POST` methods, for the moment. The `GET` method has been tested widely and appears to be consistent with the output of the `curl` CLI.
- The curl-REST wrapper uses a singleton-equipped `curl` instance to limit memory fingerprint.
- Rucio-API wrappers, based on Rucio's RESTFul interface, are being developed. At the moment access token retrieval through userpass authentication and scopes listing are implemented and tested.

## TODOs
- implement `getattr`, `readdir` and `read` methods
- surely a lot more...

## How to build

To build the software please run:

```[shell]
./build.sh
```

This has been tested on CentOS7 and Mac OS X Mojave 10.14.6 .

To complete the build `libcurl-devel`, `fuse-libs` and `fuse-devel` packages (or equivalent) must be present:
`cmake` will try to locate them for you and trigger some build messages if unable to do so.
Please note that `cmake` version 3 or greater is needed.

## Configuration
Update [`source/globals.cpp`](https://github.com/rucio/fuse-posix/blob/master/source/globals.cpp#L7) with your server's hostname and login
information for a userpass identity relevant for your deployment.  The configuration reads as:
```
{
   "rucio-server",
    rucio_server(<server https endpoint>,
                          <identity or owner>,
                          <account>,
                          <plain password>)
}
```


## Building and running in docker
Disclaimer: there are probably better ways of doing this but these are reasonably convenient for a quick test.

You can also build and test in an isolated environment with a docker image.
This option builds the software into a centos7 image and starts a container
which mounts the ruciofs inside.

Before building, edit the `globals.cpp` configuration as described above.  A container image can be built in the usual way:
```
docker build -t rucio-fuse-posix .
```

To start, run `modprobe fuse` on the **host** system and then start the
container with an additional `--device` option:
```
$ modprobe fuse
$ docker run -it  --device /dev/fuse --cap-add SYS_ADMIN rucio-fuse-posix
```
You should see output like:
```
----------------------------------------
Activating rucio fuse-posix interface...
Creating CURL instance
```
Your rucio data should then be visible in `/ruciofs`.  Attach to the running container with e.g.,
```
$ docker exec -it <container-id> bash
[root@4965bd4f018f /]# ls /ruciofs
rucio-server  rucio-server-clone
```
