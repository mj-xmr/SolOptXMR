#!/bin/bash -e

POOL_IP=$1
if [ -z $POOL_IP ]; then
	echo "Please provide the pool's IP as the 1st parameter"
	exit 1
fi

CORES=$2
if [ -z $CORES ]; then
	CORES=`nproc`
fi

POOL_PORT=$3
if [ -z $POOL_PORT ]; then
	POOL_PORT=3333
fi

cd build/xmrig/build
nice -n 19 ./xmrig --threads=$CORES --donate-level 40 -o $POOL_IP:$POOL_PORT

