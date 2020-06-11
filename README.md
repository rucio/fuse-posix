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
- Fuse mounting works!
- Rucio-API is now backed with cache, to reduce proactively the number of cURL operations.
- Introduced runtime parsing of configuration file (`settings.json`?) to replace `globals.cpp`. The syntax is clear from the included example file.
- Introduced log-level based logging using submodule `fastlog`[link](https://github.com/gabrielefronze/fastlog).
  As a side note three log levels (ERROR > INFO > DEBUG, in order of reversed gravity and direct verbosity) have been introduced and are usable at runtime:
  
  - `-v` = INFO
  - `-vv` = DEBUG
  - `<no opt>` = ERROR (default)
  
  For example `rucio-fuse-main -v` invokes the software with INFO log level, `rucio-fuse-main -vv` with DEBUG level and so on.

## TODOs
- Implement the read Fuse operation: ***at the moment the file contents are dummy***!
- Introduce certificate-base authentication mechanism.
- Fix Mac OS X mouting issues
- Surely a lot more...

## Getting Started

### Cloning the repository
Fork the repository and clone it on your local machine for development

```BASH
$ git clone https://github.com/<your-username>/fuse-posix.git
$ cd fuse-posix/
$ git remote add upstream https://github.com/rucio/fuse-posix.git
```

### Install the dependencies
Next step is to install the dependencies which shall be required to build the software.
To complete the build `libcurl-devel`, `fuse-libs` and `fuse-devel` packages (or equivalent) must be present:
`cmake` will try to locate them for you and trigger some build messages if unable to do so.
Please note that `cmake` version 3 or greater is needed.

**Ubuntu/Debian (apt)**

```BASH
$ sudo apt-get install cmake git g++ libfuse-dev libcurl4-gnutls-dev
```

**CentOS (yum)**

```BASH
$ yum install -y cmake3 libcurl-devel libfuse-devel
```

### Build the software
To build the software please run:

```[shell]
$ ./build.sh
```

### Setting up the FUSE mount

* **STEP 1** - Check FUSE version

```BASH
$ fusermount -V
fusermount version: 2.9.7
```

Also check if the `fuse` group is available on the system and your current user is a part of it or not

```BASH
$ groups $USER | grep fuse
```

If this highlights `fuse` then your current USER is a part of the `fuse` group and you may skip STEP 2. 
If not, then add the `fuse` group as explained in STEP 2.

* **STEP 2** - Add `fuse` group for current USER

```BASH
$ sudo groupadd fuse
$ sudo usermod -aG fuse $USER
```

The above step adds a new group named `fuse` and makes the current user a part of it. 
If you wish to access the FUSE mount as `root`, then you must also uncomment (or add if not present) the line `user_allow_other` in `/etc/fuse.conf` to enable root access for FUSE filesystem. 
Once this is done successfully, reload the fuse module with `modprobe` or restart the machine.

```
$ modprobe fuse
```

### Setting up Rucio-FUSE connections
Now that `fusermount` is set up on the machine, we need to set up the Rucio-FUSE connections before proceeding with the mounting process.

The Rucio FUSE module will parse the connection parameters from configuration files in the same format as typical `rucio.cfg` files. 
All the `.cfg` files found in the path pointed by the environment variable `RUCIOFS_SETTINGS_FILES_ROOT` will be parsed looking for the following required fields in the `[client]` section:
 - `rucio_host`: the server URL
 - `username`: the server username
 - `password`: the server password
 - `account`: the connection account

At the moment only `userpass` authentication method is supported.
To add a new server it is enough to import the existing `.cfg` and restart the FUSE module.

**NOTE:** The configuration files are passed at runtime to the internal routine performing rucio downloads. Do not remove them without restarting the FUSE module!

### Mounting with Rucio-FUSE mount
The FUSE mount can be either used as `root` or the current user.

* **OPTION A** - Mounting as `root`

To use FUSE as root, you need to set up fuse module as given in STEP 2, then follow the steps below.

```BASH
$ sudo mkdir /ruciofs
$ cd fuse-posix/
$ sudo ./cmake-build-debug/bin/rucio-fuse-main
```

* **OPTION B** - Mounting as current USER

This step requires root to change the ownership of the mount point from `root` and group `root` to `$USER` and group `fuse`

```BASH
$ sudo mkdir /ruciofs
$ sudo chown $USER:fuse /ruciofs
$ ./cmake-build-debug/bin/rucio-fuse-main
```

Performing the above steps successfully shall parse the `settings.json` file and mount the server to `/ruciofs` mount point.

## Extra Notes

* This has been tested on CentOS7, ~~Mac OS X Mojave 10.14.6~~, and Ubuntu 18.04 LTS.
* Mac OS X special files created by the OS FS service generate a lot of issues which should be dealt with and prevent the Fuse module from correct mounting under Mac.
