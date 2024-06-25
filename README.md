# Bloom filters implemented in C

## Installation & usage
1. Install openssl. For mac: `brew install openssl` for linux `sudo apt-get update && sudo apt-get install libssl-dev`
2. Compile the file, replace the openssl include paths to your ones `gcc -o bloom bloom.c -I/opt/homebrew/opt/openssl@3/include -L/opt/homebrew/opt/openssl@3/lib -lcrypto`
3. Run it on `input.txt` example with `./bloom`
