#! /bin/bash

for i in $(seq 1 32)
do
    ./wrench-example-bare-metal-chain ${i} 3 4.4 single_host.xml --multi --writeback
done