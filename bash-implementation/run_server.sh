#!/bin/bash -x

IP=${1:-127.0.0.1}
PORT=${2:-7890}
shift
shift
tcpserver -v -RHl0 "$@" $IP $PORT ./serverServer.sh

