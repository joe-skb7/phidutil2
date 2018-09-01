# phidutil2

## Overview

This tool is for phidget device described at [1]. Linux documentation can be
found at [2]. API is described at [3] (C, 1014_2-PhidgetInterfaceKit 0/0/4).

## Building the library

In order to build this tool, you'll need libphidget22 library installed.

1. Install dependencies:

```
    $ sudo aptitude install libusb-1.0-0-dev libusb-1.0-0 checkinstall
```

2. Download library source code. build and install:

```
    $ wget https://www.phidgets.com/downloads/phidget22/libraries/linux/libphidget22.tar.gz
    $ tar xzvf libphidget22.tar.gz
    $ rm libphidget22.tar.gz
    $ cd libphidget22-1.0.0.20180827/
    $ ./configure --prefix=/usr
    $ make -j4
    $ sudo checkinstall --install=no
    $ sudo chown -R $USER:$USER *
    $ sudo dpkg -i *.deb
```

## udev rules

In order to be able to run phidutil2 tool without sude, copy udev rules
from libphidget22 source code directory and restart udev:

```
    $ sudo cp plat/linux/udev/99-libphidget22.rules /etc/udev/rules.d/
    $ sudo udevadm control --reload-rules
    $ sudo udevadm trigger
```

  Now reconnect your phidget (disconnect and connect USB cable).

## Building the tool

Build and install Debian package:

    $ make debian
    $ sudo dpkg -i ../*.deb

## Authors

**Sam Protsenko**

## License

This project is licensed under the GPLv2.

## References

[1] https://www.phidgets.com/?tier=3&catid=46&pcid=39&prodid=1020

[2] https://www.phidgets.com/docs/OS_-_Linux

[3] https://www.phidgets.com/?view=api
