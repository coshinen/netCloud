# netCloud

[![Build Status](https://travis-ci.org/mistydew/netCloud.svg?branch=master)](https://travis-ci.org/mistydew/netCloud)

A customised protocol cloud storage.

## Todo

- [ ] code refactoring: config system
- [ ] code merge: server and client - move cli to src
- [ ] code refactoring: replace c with cpp

## To Build

```
$ git clone https://github.com/mistydew/netCloud.git
$ cd netCloud
$ make
$ make install # optional
```

## Dependencies

These dependencies are required:

Library        | Purpose | Description
---------------|---------|-------------
libssl         | Crypto  | Random Number Generation, Elliptic Curve Cryptography
mysql          | MySQL   | Data storage, Cache
libmysqlclient | MySQL   | Database operating

## Linux Distribution Specific Instructions

### Ubuntu

#### Dependency Build Instructions

Build requirements:

##### MySQL

```
$ sudo apt install mysql-server mysql-client
$ sudo apt install libmysqlclient-dev
```

## Usage

```
$ ./ftpd
$ ./ftp-cli
```
