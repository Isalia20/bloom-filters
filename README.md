# Bloom Filters in C

This project implements Bloom filters in C

## Installation

### Prerequisites

- OpenSSL library

#### macOS
```bash
brew install openssl
```

#### Linux
```
sudo apt-get update && sudo apt-get install libssl-dev
```

#### Compilation
```
gcc -Wno-deprecated-declarations -o bloom bloom.c -I/opt/homebrew/opt/openssl@3/include -L/opt/homebrew/opt/openssl@3/lib -lcrypto
```

#### Usage
```
./bloom
```
