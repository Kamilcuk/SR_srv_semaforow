#!/bin/bash -xe

sudo tcpdump -Ani lo  port 7890 or port 7891 or port 7892 or port 7893 or port 7894 | tee /tmp/tcpdump.txt

