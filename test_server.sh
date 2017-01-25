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
	local writedesc=${2:-3}
	local readdesc=${3:-4}
	shift 3
        local fw=$tmpdir/fw$writedesc
        local fr=$tmpdir/fr$readdesc
        mkfifo $fr $fw
        ( set -x; "$@" ) <$fw >$fr &
	new_child=$!
        eval "exec ${writedesc}>$fw"
       	eval "exec ${readdesc}<$fr"
	export new_child
}

        ctrl_c() { 
		(
			set -x
			echo "CTRL_C function!" || true
	               kill $child || true
	               rm -fr $tmpdir || true
		)
        }
        readRECV() {
                while read -t 1 line; do
                        [ "$READRECVDEBUG" = true ] && echo "PROGRAM: $line" >&2
                        if [[ $line =~ ^Recv: ]]; then
                                echo $line | sed 's/Recv:[ ]*//'
                                return
                        fi
                done
        }
        sendrecv() {
		descw=${2:-3}
		descr=${3:-4}
                echo "$1" >&${descw}
                recv=$(readRECV <&${descr})
                echo -e "v-> $1 -- ($2,$3) \n^-> $recv\n"
                export recv
        }

check_port_is_open() {
	port=$1
	netstat -lnt | awk '$6 == "LISTEN" && $4 ~ ".:'$port'"'
}

client_mode1() {
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

client_mode2() {
	READRECVDEBUG=true
	export READRECVDEBUG
	# some deadlock
        SERVERURL=${1:-127.0.0.1:7890}
        PORT1=${2:-$((7890+${RANDOM}%100))}
        PORT2=${3:-$((7890+${RANDOM}%100))}
	PORT3=${3:-$((7890+${RANDOM}%100))}
        tmpdir=$(mktemp -d)
	child=""
	recv=""
        trap 'ctrl_c' SIGINT
        trap 'ctrl_c' EXIT

	new_child=""
	run_child $tmpdir 3 4 ./SR_srv_semaforow --type client --listenport $PORT1 --listenip 127.0.0.1 --serverurl $SERVERURL
	child+=" ${new_child} "
        run_child $tmpdir 5 6 ./SR_srv_semaforow --type client --listenport $PORT2 --listenip 127.0.0.1 --serverurl $SERVERURL
        child+=" ${new_child} "
	run_child $tmpdir 7 8 ./SR_srv_semaforow --type client --listenport $PORT3 --listenip 127.0.0.1 --serverurl $SERVERURL
	child+=" ${new_child} "
	export child
	export tmpdir

	sleep 2;
	if ! check_port_is_open $PORT1 || ! check_port_is_open $PORT2 || ! check_port_is_open $PORT3; then
		echo "BUG EEOR"
		exit 1
	fi

	sendrecv 'Maintenance.Hello Bash_Test_Script' 3 4
	
	# lets create two sems
        sendrecv 'Locks.CreateSemaphore 1 0 1' 3 4
        sem1=$recv
        sendrecv 'Locks.CreateSemaphore 1 0 1' 3 4
        sem2=$recv

        sendrecv "Locks.Dump" 3 4

	# decrement both in bad order
        sendrecv "Locks.DecrementSemaphore $sem1" 3 4
	sendrecv "Locks.DecrementSemaphore $sem2" 5 6

	# deadlock for both - (3 4) decremented 1 waits for 2, (5 6) otherwise
	sendrecv "Locks.DecrementSemaphore $sem2" 3 4
	sendrecv "Locks.DecrementSemaphore $sem1" 5 6
	
	# another client start Locks.Probe on sem1
	sendrecv "Locks.Dump" 7 8 # dump it first
	sendrecv "Locks.Probe 127.0.0.1:$PORT3 $sem1" 7 8

        sleep 10 # we should receive some DEADLOCK DEADLOCK messages
        sendrecv "Locks.Dump" 7 8

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


