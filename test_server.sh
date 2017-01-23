#!/bin/bash

PORT=${1:-7890}

echo "$0: PORT=$PORT"

send() {
	echo "\"$1\" -> \"$( command curl \
	-H "Content-Type: application/json" \
	-X POST -d "$1" http://localhost:$PORT/v1 2>/dev/null
	)\""
}

send 'gupi test'
send '{"password":"xyz","username":"xyz"}'
send '{"method":"Maintenance.Hello","params":[{"Name":"nazwa"}]}'


