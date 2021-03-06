#!/bin/bash -e

kamilclientexe="${kamilclientexe:-./SR_srv_semaforow}"
readRECVtimeout=2;

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

_run_child_iterator=1
run_child() {
	local tmpdir=$1
	local writedesc=$(( ${2:-3}*2+11 ))
	local readdesc=$(( ${2:-3}*2+12 ))
	shift 2
    local fw=$tmpdir/fw$writedesc
    local fr=$tmpdir/fr$readdesc
    mkfifo $fr $fw
    ( ( set -x; "$@" <$fw >$fr ) 2>&1 | sed "s;^;STDERR_${_run_child_iterator}: ;" 1>&2 ) &
    _run_child_iterator=$((_run_child_iterator+1))
	new_child=$!
    eval "exec ${writedesc}>$fw"
    eval "exec ${readdesc}<$fr"
	export new_child
}

kill_all_childs() {
	kill -- -$(ps -o pgid= $$ | grep -o '[0-9]*')
	sleep 0.5
	kill -9 -$(ps -o pgid= $$ | grep -o '[0-9]*')
}

ctrl_c() { 
(
	set -x
	echo "CTRL_C function!" || true
    kill $child || true
	kill_all_childs || true
    rm -fr $tmpdir || true
)
}
readRECV() {
	local identifier="${1:- }"
        while read -t "${readRECVtimeout}" line; do
                [ "$READRECVDEBUG" = true ] && echo "STDOUT_${identifier}: $line" >&2
                if [[ $line =~ ^Recv: ]]; then
                        echo $line | sed 's/Recv:[ ]*//'
                        return
                fi
        done
}
sendrecv() {
	local text="${1:-}"
	local descw=${2:-3}
	local descr=${3:-4}
	local number=$(( ( ${descw} - 11 ) / 2 ))
    echo "v_${number}-> $text -- _${number}($2,$3)"
    [ -n "$text" ] && echo "$text" >&${descw}
    recv=$(readRECV $number <&${descr})
    echo "^_${number}-> $recv"
    echo;
    export recv
}

check_port_is_open() {
	local port=$1
	netstat -lnt | awk '$6 == "LISTEN" && $4 ~ ".:'$port'"'
}

sendrecvchild() {
	local number=${1}; shift;
	sendrecv "$@" $((${number}*2+11)) $((${number}*2+12))
}

client_mode1() {
	SERVERURL=${1:-127.0.0.1:7890}
	PORT1=${2:-$((10000+${RANDOM}%1000))}
	PORT2=${3:-7900}
    tmpdir=$(mktemp -d)
	child=""
	recv=""
    trap 'ctrl_c' SIGINT
    trap 'ctrl_c' EXIT

	run_child $tmpdir 1 ${kamilclientexe} --type client --listenport $PORT1 --listenip 127.0.0.1 --serverurl $SERVERURL
	child+=" ${new_child} "
	export child
	export tmpdir
	sleep 4

	sendrecvchild 1 'Maintenance.Hello Bash_Test_Script'
	sendrecvchild 1 'Locks.CreateSemaphore 2 0 2'
	sem1=$recv
	sendrecvchild 1 "Locks.Dump"
	sendrecvchild 1 "Locks.DecrementSemaphore $sem1"
	sendrecvchild 1 "Locks.Probe 127.0.0.1:$PORT1 127.0.0.1:$PORT1 $sem1"
	sleep 2
	sendrecvchild 1 "Locks.Dump"
	sendrecvchild 1 "Locks.DeleteSemaphore $sem1"
	sleep 2
	sendrecvchild 1 "quit"
	echo
	ctrl_c
}

client_mode2() {
	READRECVDEBUG=true
	export READRECVDEBUG
	# some deadlock
    SERVERURL=${1:-127.0.0.1:7890}
	PORT1=${2:-$((10000+${RANDOM}%1000))}
    PORT2=${3:-$((10000+${RANDOM}%1000))}
	PORT3=${3:-$((10000+${RANDOM}%1000))}
    tmpdir=$(mktemp -d)
	child=""
	recv=""
    trap 'ctrl_c' SIGINT
    trap 'ctrl_c' EXIT

	new_child=""
	run_child $tmpdir 1 ${kamilclientexe} --type client --listenport $PORT1 --listenip 127.0.0.1 --serverurl $SERVERURL
	child+=" ${new_child} "
    run_child $tmpdir 2 ${kamilclientexe} --type client --listenport $PORT2 --listenip 127.0.0.1 --serverurl $SERVERURL
    child+=" ${new_child} "
	run_child $tmpdir 3 ${kamilclientexe} --type client --listenport $PORT3 --listenip 127.0.0.1 --serverurl $SERVERURL
	child+=" ${new_child} "
	export child
	export tmpdir

	echo "Testing clients"
	for i in 1 2 3; do
		sendrecvchild ${i} ""
	done
	for i in 1 2 3; do
		sendrecvchild ${i} 'Maintenance.Hello Bash_Test_Script'
	done
	for i in 1 2 3; do
		sendrecvchild ${i} ""
	done
	if ! check_port_is_open $PORT1 || ! check_port_is_open $PORT2 || ! check_port_is_open $PORT3; then
		echo "BUG EEOR"
		exit 1
	fi
	
	# lets create two sems
    sendrecvchild 1 'Locks.CreateSemaphore 1 0 1'
    sem1=$recv
    sendrecvchild 1 'Locks.CreateSemaphore 1 0 1'
    sem2=$recv

    sendrecvchild 1 "Locks.Dump"

	# decrement both in bad order
    sendrecvchild 1 "Locks.DecrementSemaphore $sem1"
	sendrecvchild 2 "Locks.DecrementSemaphore $sem2"

	# deadlock for both - (3 4) decremented 1 waits for 2, (5 6) otherwise
	sendrecvchild 1 "Locks.DecrementSemaphore $sem2"
	sendrecvchild 2 "Locks.DecrementSemaphore $sem1"
	
	# another client start Locks.Probe on sem1
	sendrecvchild 3 "Locks.Dump" # dump it first
	sleep 1

	sendrecvchild 3 "Locks.Probe 127.0.0.1:$PORT1 127.0.0.1:$PORT1 $sem2"
    sleep 2 # we should receive some DEADLOCK DEADLOCK messages

	sendrecvchild 3 "Locks.Probe 127.0.0.1:$PORT2 127.0.0.1:$PORT2 $sem1"
	sleep 2

    sendrecvchild 2  "Locks.Dump"

    for i in 1 2 3; do
    	sendrecvchild $i 'quit'
    done
	sleep 2

	ctrl_c
	wait
	exit 0 # cleanup ? crap - just kill'em all
}


case "$1" in
raw_mode*|client_mode*)
	func=$1
	shift
	"$func" "$@"
	;;
*)
	raw_mode1 "$@"
	;;
esac


