#!/bin/bash
mkdir -p build
g++ -o ./build/prontohexparser BurstPair.cpp ProntoHex.cpp main.cpp
