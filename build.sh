#!/usr/bin/env bash
set -xeo pipefail
g++ main.cpp -Wall -Wextra -o soft-rasterization
./soft-rasterization