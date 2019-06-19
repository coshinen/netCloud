# netCloud

A customised protocol cloud storage.

## To Build

```
$ git clone https://github.com/mistydew/netCloud.git
$ cd netCloud
$ make
$ make install # optional
```

## Dependencies

These dependencies are required:

Library| Purpose | Description
-------|---------|-------------
libssl | Crypto  | Random Number Generation, Elliptic Curve Cryptography
mysql  | MySQL   | Data storage

## Linux Distribution Specific Instructions

### Ubuntu

#### Dependency Build Instructions

Build requirements:

##### MySQL

```
$ sudo apt install mysql-server mysql-client
```

## Usage

```
$ ./ftps ~/netCloud/ftps.conf
$ ./ftpc
```
