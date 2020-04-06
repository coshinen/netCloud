# netCloud

![](https://github.com/mistydew/netCloud/workflows/C/C++%20CI/badge.svg)

A custom cloud storage server and client.

## Todo

- [ ] code merge: server and client - move cli to src
- [ ] code refactoring: config, logging system, db (replace mysql with bdb or leveldb)

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
openssl        | Crypto  | Random Number Generation, Elliptic Curve Cryptography
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
$ ./netcloud
$ ./netcloud-cli
```

## License

netCloud is released under the terms of the MIT license.
See [LICENSE](LICENSE) for more information or see https://opensource.org/licenses/MIT.
