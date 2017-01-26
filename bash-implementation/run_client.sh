#!/bin/bash

tcpserver -v -RHl0 "$@" 127.0.0.1 7891 ./client.sh

