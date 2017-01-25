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

run_child() {
	local tmpdir=$1
	local writedesc=$2
	local readdesc=$3
	shift 3
        local fw=$tmpdir/fw$writedesc
        local fr=$tmpdir/fr$readdesc
        mkfifo $fr $fw
        ( set -x; "$@" ) <$fw >$fr &
	local child=$!
        exec 3>$fw
        exec 4<$fr
	echo -ne "$child"
}


client_mode() {
	SERVERURL=${1:-127.0.0.1:7890}
	PORT1=${2:-$((7890+${RANDOM}%100))}
	PORT2=${3:-7900}

	tmpdir=$(mktemp -d)
	fw=$tmpdir/fw
	fr=$tmpdir/fr
	mkfifo $fr $fw
	( set -x; ./SR_srv_semaforow --type client --listenport $PORT1 --listenip 127.0.0.1 --serverurl $SERVERURL ) <$fw >$fr &
	child=$!
	exec 3>$fw
	exec 4<$fr
	ctrl_c() { 
		kill $child
		rm -r $tmpdir
	}
	readRECV() {
		while read -t 1 line; do
			echo "PROGRAM: $line" >&2
			if [[ $line =~ ^Recv: ]]; then
				echo $line | sed 's/Recv:[ ]*//'
				return
			fi
		done
	}
	sendrecv() {
		echo "$1" >&3
		recv=$(readRECV <&4)
		echo -e "v-> $1 \n^-> $recv\n"
		export recv
	}

	trap 'ctrl_c' SIGINT

	recv=""

	sendrecv 'Maintenance.Hello Bash_Test_Script'
	sendrecv 'Locks.CreateSemaphore 2 0 2'
	sem1=$recv
	sendrecv "Locks.Dump"
	sendrecv "Locks.DecrementSemaphore $sem1"
	sendrecv "Locks.Probe 127.0.0.1:$PORT1 $sem1"
	sleep 2
	sendrecv "Locks.Dump"
	sendrecv "Locks.DeleteSemaphore $sem1"

	echo
	ctrl_c
}

case "$1" in
client)
	shift
	client_mode "$@"
	;;
raw_mode*)
	func=$1
	shift
	"$func" "$@"
	;;
*)
	raw_mode1 "$@"
	;;
esac


