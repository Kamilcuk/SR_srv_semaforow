#!/bin/bash -e

PORT=${1:-7890}

nc -z localhost $PORT # will quit on no connection, becouse set -e :p

echo "$0: PORT=$PORT"

send() {
	output=$(command curl \
		-H "Content-Type: application/json" \
		-X POST -d "$1" http://localhost:$PORT/v1 2>/dev/null)
	ret=$?
	if [[ $ret -ne 0 ]]; then
		return;
	fi
	echo "\"$1\"" >&2
       	echo "^-> \"$output\"" >&2
	echo "$output"
}

sendnull() {
	send "$@" > /dev/null
}

sendnull 'gupi test'
sendnull '{"password":"xyz","username":"xyz"}'
sendnull '{"method":"Maintenance.Hello","params":[{"Name":"nazwa"}]}'
sendnull '{"method":"Maintenance.Heartbeat"}'
out=$( send '{"method":"Locks.CreateSemaphore","params":[{"CurrentVal":"1","MinVal":"0","MaxVal":"1"}]}' )
uuid=$(echo "$out" | jq -r '.params[0].UUID')
sendnull '{"method":"Locks.Dump"}'
sendnull '{"method":"Locks.DeleteSemaphore","params":[{"UUID":"'"$uuid"'"}]}'



