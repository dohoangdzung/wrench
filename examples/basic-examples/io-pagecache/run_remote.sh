#! /bin/bash

for i in $(seq 1 32)
do
    ./wrench-example-bare-metal-chain-remote ${i} 3 4.4 two_hosts.xml --writeback
done