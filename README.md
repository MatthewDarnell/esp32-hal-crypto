# ESP32-hal-crypto

A library using the hardware accelerated cryptographic tools onboard the ESP32

## Description

The ESP32 has dedicated onboard hardware acceleration registers for various cryptographic tools, including:

1. SHA Hashes
2. True (and Pseudo) Random Number Generator (RNG)
3. AES Encryption
4. RSA

This project is currently producing a static library, `libesp-hal-crypto.a`, and a simple executable that SHA256 hashes a string. Feel free to use it as a template for your executable project.
## Getting Started

### Dependencies

* ESP-IDF toolchain installation
* CMake

### Installing

* Start by following the guide for installing the esp idf toolchain found [here](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#manual-installation)
* After installation, can source the toolchain in `.bashrc` with this: `echo 'source $HOME/esp/esp-idf/export.sh' >> ~/.bashrc`

  This sets up the `IDF_PATH` env var required for CMake to find the toolchain.
* If you are using CLion IDE, [this](https://developer.espressif.com/blog/clion/) is a helpful guide.
* If you are using command line, you can set your CMake to use the esp-idf toolchain with the flag `cmake -DCMAKE_TOOLCHAIN_FILE=$HOME/path/to/esp/esp-idf/tools/cmake/toolchain-esp32.cmake`
### Building The Library

* From IDE:
 ```cmake
cmake --build $HOME/CLionProjects/esp32-hal-crypto/cmake-build-debug --target esp-hal-crypto -j 8
````
This will place `libesp-hal-crypto.a` into the `cmake-build-debug/esp-idf/main/`

### Running The Application
Following the linked guides above will help you to flash and run the application on your board.

The `esp-idf` toolchain has a `flash` target that your ide can run. You can also run `idf.py flash monitor` to flash and run the application, just remember to source the `export.sh` file in your terminal first!

## Notes

Currently only RNG and SHA256 are implemented.

Hashing `The quick brown fox jumps over the lazy dog` yielded 107kH/s on my board.

There is no warranty on this software.

Feel free to open a pull request if you find a bug or would like to add additional functionality.

## Authors

Matthew Darnell
