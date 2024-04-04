# Simple Server/Client Socket

Basic implementation of TCP client/server for transmitting byte data. Expect bugs, do not use in prod.

## Requirements

See `launch.sh` and `CMakeLists.txt`.

1. Private 32 byte password saved to `USER_SOCKET_KEY` env variable. This will be changed in the future.
2. `libsodium`
3. `linux`

## Features

This project provides an file transfer implementation that:

1. Uses (ghetto) syn/ack for confirmation
2. Encrypts file data with `libsodium` via a known private key

## WIPs

1. Better file handlers (save demo, copy when complete or delete otherwise)
2. Two-way transfer
3. Online authentication
