#! /bin/bash

# Original WRENCH
#for i in $(seq 1 32)
#do
#    ./wrench-example-io-pagecache-nfs ${i} 3 4.4 two_hosts.xml
#done

# WRENCH with page cache
for i in $(seq 1 32)
do
    ./wrench-example-io-pagecache-nfs ${i} 3 4.4 two_hosts.xml --writeback --wrench-full-log
done