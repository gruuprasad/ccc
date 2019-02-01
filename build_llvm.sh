#!/usr/bin/env bash
set -euP

COLOR="\e[33m"
_COLOR="\e[0m"

if [ ! -e  "llvm" ]; then

echo -e "$COLOR===> mkdir llvm...$_COLOR"
mkdir -p llvm
cd llvm
CUR=$PWD
echo -e "$COLOR===> DONE.$_COLOR"

cho -e "$COLOR===> get sources...$_COLOR"
if [ ! -e  "$CUR/src" ]; then
        echo -e "$COLOR===> get sources::llvm...$_COLOR"
        wget http://releases.llvm.org/7.0.1/llvm-7.0.1.src.tar.xz
        tar xf llvm-7.0.1.src.tar.xz
        rm llvm-7.0.1.src.tar.xz
        mv llvm-7.0.1.src src
        echo -e "$COLOR===> get sources::clang...$_COLOR"
        wget http://releases.llvm.org/7.0.1/cfe-7.0.1.src.tar.xz
        tar xf cfe-7.0.1.src.tar.xz
        rm cfe-7.0.1.src.tar.xz
        mv cfe-7.0.1.src src/tools/clang
        cd "$CUR"
fi
echo -e "$COLOR===> DONE.$_COLOR"

echo -e "$COLOR===> make...$_COLOR"
mkdir -p build
mkdir -p install
cd build
cmake ../llvm \
        -DCMAKE_BUILD_TYPE:STRING=Release \
        -DCMAKE_INSTALL_PREFIX:PATH=$CUR/install \
        -DLLVM_ENABLE_ASSERTIONS:BOOL=ON \
        -DLLVM_ENABLE_EH:BOOL=ON \
        -DLLVM_ENABLE_RTTI:BOOL=ON \
        -DLLVM_INCLUDE_TESTS:BOOL=OFF \
        -DLLVM_TARGETS_TO_BUILD="X86"
echo -e "$COLOR===> DONE.$_COLOR"

echo -e "$COLOR===> install...$_COLOR"
make -j$(nproc) install
echo -e "$COLOR===> DONE.$_COLOR"

else

echo -e "$COLOR===> cache llvm...$_COLOR"
echo -e "$COLOR===> DONE.$_COLOR"

fi
