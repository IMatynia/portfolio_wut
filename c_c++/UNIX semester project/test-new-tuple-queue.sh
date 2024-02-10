#!/bin/sh

binary="./build/daemon-test-tool"

"$binary" --create

seq 1 1000 | parallel --jobs 100 "$binary" --enqueue '{}' &
seq 1 1000 | parallel -N0 --jobs 100 "$binary" --dequeue &

wait
wait

"$binary" --delete
