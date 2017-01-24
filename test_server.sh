#!/bin/bash -e

############################ functions ###########################
send() {
        output=$(command curl \
                -H "Content-Type: application/json" \
                -X POST -d "$1" http://localhost:$PORT/v1 2>/dev/null)
        ret=$?
        if [[ $ret -ne 0 ]]; then
                return;
        fi
        echo -ne "SEND: \"$1\" \nRECV \"$output\"\n\n" >&2
        echo "$output"
}

sendnull() {
        send "$@" > /dev/null
}

_id=0
getid() { _id=$((_id+1)); echo -n ",\"id\":\"$_id\""; }

sem_new() {
        send '{"id":"1","method":"Locks.CreateSemaphore","params":[{"CurrentVal":'${1:-1}',"MinVal":'${2:-0}',"MaxVal":'${3:-1}'}]}' | jq -r '.result.UUID'
}
sem_del() {
        send '{"id":"1","method":"Locks.DeleteSemaphore","params":[{"UUID":"'"$1"'"}]}'
}
sem_inc() {
        send '{"id":"1","method":"Locks.IncrementSemaphore","params":[{"UUID":"'"$1"'","Owner":"'${2}'"}]}'
}
sem_dec() {
        send '{"id":"1","method":"Locks.DecrementSemaphore","params":[{"UUID":"'"$1"'","Owner":"'${2}'"}]}'
}
locks_dump() {
        sendnull '{"id":"1","method":"Locks.Dump","params":[{}]}'
}


raw_mode1() {

PORT=${1:-7890}
OWNER=${2:-127.0.0.1:7891}

nc -z localhost $PORT # will quit on no connection, becouse set -e :p
echo "$0: PORT=$PORT"

#sendnull 'gupi test'
#sendnull '{"password":"xyz","username":"xyz"}'
sendnull '{"id":"1","method":"Maintenance.Hello","params":[{"Name":"nazwa"}]}'
sendnull '{"id":"1","method":"Maintenance.Heartbeat","params":[{}]}'
sem1=$(sem_new 2 0 2)
locks_dump
sem_dec $sem1 "$OWNER";locks_dump
sem_inc $sem1 "$OWNER";locks_dump
sem_inc $sem1 "$OWNER";locks_dump
sem_inc $sem1 "$OWNER";locks_dump
sem_dec $sem1 "$OWNER";locks_dump
sem_dec $sem1 "$OWNER";locks_dump
( sem_dec $sem1 "$OWNER";locks_dump; ) &
sleep 1
( sem_inc $sem1 "$OWNER";locks_dump; ) &
sleep 1
sem_inc $sem1 "$OWNER";locks_dump
sem_inc $sem1 "$OWNER";locks_dump
wait
sem_del $sem1


# {"method":"Locks.CreateSemaphore","params":[{"CurrentVal":"1","MinVal":"0","MaxVal":"1"}]}'
# {"method":"Locks.DeleteSemaphore","params":[{"UUID":"8321bd22-7c74-4ab3-b0e5-afa9c141011d"}]}
# {"method":"Locks.DecrementSemaphore","params":[{"UUID":"0556e503-5bba-44a3-9447-bae8a5fb0636","Owner":"127.0.0.1:7894"}]}'

}

raw_mode2() {
PORT=${1:-7890}
OWNER=${2:-127.0.0.1:7891}

sem1=$(sem_new 1 0 2)
sem2=$(sem_new 1 0 2)
sem3=$(sem_new 1 0 2)
sem_dec $sem1 "$OWNER";locks_dump
sem_dec $sem2 "$OWNER";locks_dump
sem_dec $sem3 "$OWNER";locks_dump
( sem_dec $sem1 "$OWNER";locks_dump; ) &
sleep 1
( sem_dec $sem2 "$OWNER";locks_dump; ) &
( sem_dec $sem3 "$OWNER";locks_dump; ) &
sleep 1
( sem_inc $sem2 "$OWNER";locks_dump; ) &
sleep 1
( sem_inc $sem3 "$OWNER";locks_dump; ) &
sleep 1
( sem_inc $sem1 "$OWNER";locks_dump; ) &
sleep 1
wait 
sem_del $sem1
sem_del $sem2
sem_del $sem3

}

case "$1" in
raw_mode*)
	func=$1
	shift
	"$func" "$@"
	;;
*)
	raw_mode1 "$@"
	;;
esac


