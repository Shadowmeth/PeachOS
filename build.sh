#/bin/bash

export PREFIX="/opt/gcc-16.1.0/"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
export PREFIX="/opt/binutils-2.47/"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

make all
