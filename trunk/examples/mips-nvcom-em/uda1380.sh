#!/bin/sh

dd if=test-uda1380.bin > img.bin
dd if=wave.wav >> img.bin
mcprog img.bin 0x1fc00000

