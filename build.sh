#!/usr/bin/env bash
set -xeo pipefail
g++ math.cpp rasterizer.cpp main.cpp -std=c++17 -Wall -Wextra -o soft-rasterization
./soft-rasterization