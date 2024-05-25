#!/bin/zsh

set -x

# Set the current working directory to ../source/modules
cd ../source/modules

# Compile each .cpp file into a .pcm file
for file in *.cpp; do
    clang++ -std=c++23 -x c++-module "$file" --precompile -o "pcm/${file%.cpp}.pcm"
done

cd ../

# Compile main_osx.cpp with the prebuilt modules and output to ../build/forgette_osx
clang++ -std=c++23 main_osx.cpp -fprebuilt-module-path=./modules/pcm ./modules/pcm/*.pcm -o ../build/forgette_osx
