#!/bin/bash -e

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

client_mode2() {
	READRECVDEBUG=true
	export READRECVDEBUG
	# some deadlock
    SERVERURL=${1:-127.0.0.1:7890}
    PORT1=${2:-$((7990+${RANDOM}%100))}
    PORT2=${3:-$((7990+${RANDOM}%100))}
	PORT3=${3:-$((7990+${RANDOM}%100))}
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
	sendrecv "Locks.Probe 127.0.0.1:$PORT1 $sem2" 7 8

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


