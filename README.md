Myriad Core integration/staging tree
========================================

http://www.myriadcoin.org

Copyright (c) 2009-2017 Bitcoin Core Developers
Copyright (c) 2014-2017 Myriad Core Developers

Myriad is an experimental digital currency that enables instant payments to
anyone, anywhere in the world. Bitcoin uses peer-to-peer technology to operate
with no central authority: managing transactions and issuing money are carried
out collectively by the network. Myriad Core is the name of open source
software which enables the use of this currency.

For more information, as well as an immediately useable, binary version of
the Myriad Core software, see http://www.myriadcoin.org, or read the
[original bitcoin whitepaper](https://bitcoincore.org/bitcoin.pdf).

License
-------

Myriad Core is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

Building
--------

See files in the [doc](doc) directory for generic build instructions for Windows,
OSX and Unix. Basic requirements:

* GCC and binuntils
* Git
* Boost C++ libraries
* Berkeley DB - Should be version 4.8, newer versions are known to work but not guaranteed
* libssl (part of openssl)
* [miniupnpc](http://miniupnp.free.fr/) for optional UPNP support

Requirements to build the GUI:

* QT4 or QT5 development libraries
* protobuf development libraries
* libqrencode development libraries

A common question is how to build from Ubuntu, instructions follow:

1. sudo add-apt-repository ppa:bitcoin/bitcoin -y
2. sudo apt-get update
3. sudo apt-get install build-essential curl git libminiupnpc-dev libssl-dev m4 -y
4. sudo apt-get install libdb4.8-dev libdb4.8++-dev -y
5. sudo apt-get install libprotoc-dev libprotobuf-dev -y
6. sudo apt-get install libtool automake autoconf make pkg-config -y
7. sudo apt-get install libpng-dev -y
8. sudo apt-get install libqrencode-dev -y
9. sudo apt-get install libqt4-dev -y
10. sudo apt-get install libboost-all-dev -y
11. sudo apt-get install libcurl4-openssl-dev -y
12. git clone git://github.com/myriadteam/myriadcoin.git
13. cd myriadcoin
14. ./autogen.sh
15. ./configure
16. make

Other Build Notes:

If you are compiling yourself, please configure with something like this:
```
CFLAGS="-O2 -fPIC" CPPFLAGS="-O2 -fPIC" ./configure
```
otherwise you'll probably get some errors later on. Additionally, if your CPU supports SSE2, and most modern CPU's do, use this:
```
CFLAGS="-O2 -fPIC -DUSE_SSE2" CPPFLAGS="-O2 -fPIC -DUSE_SSE2" ./configure
```
That will enable the SSE2 version of the Scrypt algorithm. This may reduce the CPU load when syncing the blockchain.


Development tips and tricks
---------------------------

**compiling for debugging**

Developers are strongly encouraged to write [unit tests](src/test/README.md) for new code, and to
submit new unit tests for old code. Unit tests can be compiled and run
(assuming they weren't disabled in configure) with: `make check`. Further details on running
and extending unit tests can be found in [/src/test/README.md](/src/test/README.md).

There are also [regression and integration tests](/qa) of the RPC interface, written
in Python, that are run automatically on the build server.
These tests can be run (if the [test dependencies](/qa) are installed) with: `qa/pull-tester/rpc-tests.py`

The Travis CI system makes sure that every pull request is built for Windows, Linux, and OS X, and that unit/sanity tests are run automatically.

The -debug=... command-line option controls debugging; running with just -debug will turn
on all categories (and give you a very large debug.log file).

The Qt code routes qDebug() output to debug.log under category "qt": run with -debug=qt
to see it.

**testnet and regtest modes**

Run with the -testnet option to run with "play myriad" on the test network, if you
are testing multi-machine code that needs to operate across the internet.

If you are testing something that can run on one machine, run with the -regtest option.
In regression test mode blocks can be created on-demand; see qa/rpc-tests/ for tests
that run in -regest mode.

**DEBUG_LOCKORDER**

Myriad Core is a multithreaded application, and deadlocks or other multithreading bugs
can be very difficult to track down. Compiling with -DDEBUG_LOCKORDER (configure
CXXFLAGS="-DDEBUG_LOCKORDER -g") inserts run-time checks to keep track of what locks
are held, and adds warning to the debug.log file if inconsistencies are detected.
