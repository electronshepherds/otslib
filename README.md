## About The Project
otslib is an implementation of the Bluetooth Object Transfer Service (OTS) Client. It is a work in progress and currently does not support all functionality outlined by the [OTS Specification](https://www.bluetooth.com/specifications/specs/object-transfer-service-1-0/).

### Built With
* [bluez](http://www.bluez.org/)
* [gattlib](https://github.com/labapart/gattlib)

## Getting Started
The library can be incorporated into a yocto build system using the [meta-electronshepherds](https://github.com/electronshepherds/meta-electronshepherds) bitbake layer. Alternatively it can be built manually using the following steps.

Further documentation for the library can be found [here](https://electronshepherds.github.io/otslib/).

### Prerequisites
To build the library some minimal requirements are needed.

#### bluez
The needed bluez requirement can be satisfied by installing the bluetooth development package:
```sh
sudo apt-get install libbluetooth-dev
```

#### gattlib
The needed gattlib requirement is a bit harder to satisfy. You must build and install the library manually:
1. Get gattlib
```sh
git clone https://github.com/labapart/gattlib.git
```
2. Build gattlib
```sh
cd gattlib
mkdir build
cd build
cmake -DGATTLIB_BUILD_DOCS=OFF -DCMAKE_BUILD_TYPE=Release ..
make
```
3. Install gattlib
```sh
sudo make install
```

### Build and Install
Before you can use the library you must build it.
1. Get otslib
```sh
git clone https://github.com/electronshepherds/otslib.git
```
2. Build otslib
```sh
mkdir build
cd build
cmake ..
make
```
3. Install otslib
```sh
sudo make install
```

## Usage
Usage examples can be found in the [examples directory](https://github.com/electronshepherds/otslib/tree/main/examples).

## Contributing
Contributions are welcome! Please submit a pull request if you would like to contribute to this project.

## License
Distributed under the MIT License. See `LICENSE.txt` for more information.

## Acknowledgments
