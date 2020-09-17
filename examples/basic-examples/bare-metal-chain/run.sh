#! /bin/bash

for i in $(seq 1 32)
do
    ./wrench-example-bare-metal-chain ${i} single_host.xml --wrench-full-log
done