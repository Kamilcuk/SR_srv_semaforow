#!/bin/bash -e

PORT=${1:-7890}
OWNER=${2:-127.0.0.1:7891}

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

sem_new() {
	send '{"method":"Locks.CreateSemaphore","params":[{"CurrentVal":"'${1:-1}'","MinVal":"'${2:-0}'","MaxVal":"'${3:-1}'"}]}' | jq -r '.params[0].UUID'
}
sem_del() {
	send '{"method":"Locks.DeleteSemaphore","params":[{"UUID":"'"$1"'"}]}'
}
sem_inc() {
	send '{"method":"Locks.IncrementSemaphore","params":[{"UUID":"'"$1"'","Owner":"'${2}'"}]}'
}
sem_dec() {
	send '{"method":"Locks.DecrementSemaphore","params":[{"UUID":"'"$1"'","Owner":"'${2}'"}]}'
}
locks_dump() {
	sendnull '{"method":"Locks.Dump"}'
}

#sendnull 'gupi test'
#sendnull '{"password":"xyz","username":"xyz"}'
#sendnull '{"method":"Maintenance.Hello","params":[{"Name":"nazwa"}]}'
#sendnull '{"method":"Maintenance.Heartbeat"}'
sem1=$(sem_new)
locks_dump
sem_dec $sem1 "$OWNER"
locks_dump
#sem_inc $sem1 "$OWNER"
#locks_dump
sem_del $sem1


# {"method":"Locks.CreateSemaphore","params":[{"CurrentVal":"1","MinVal":"0","MaxVal":"1"}]}'
# {"method":"Locks.DeleteSemaphore","params":[{"UUID":"8321bd22-7c74-4ab3-b0e5-afa9c141011d"}]}
# {"method":"Locks.DecrementSemaphore","params":[{"UUID":"0556e503-5bba-44a3-9447-bae8a5fb0636","Owner":"127.0.0.1:7894"}]}'











