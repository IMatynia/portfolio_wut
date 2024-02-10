#!/bin/sh

binary="./build/daemon-test-tool"

"$binary" --create

seq 1 1000 | shuf | parallel --jobs 100 "$binary" --add '{}'

"$binary" --print | sort -n

seq 1 1000 | shuf | parallel --jobs 100 "$binary" --remove '{}'

"$binary" --print | sort -n

"$binary" --delete
