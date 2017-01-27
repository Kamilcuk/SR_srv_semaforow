#!/bin/bash

set -o pipefail

##################################### vars that can change ##########################

sr_id="${sr_id:-1}"
sr_server="localhost:7890"
sr_client="localhost:7891"
## semaphores array
# array of n strings
# every string has `typeset -p s` output
# like this= 'declare -a s=([0]="uuid-uuid-uuid-uuid-uuid" [1]="127.0.0.1:7890" [2]="0" [3]="1" [4]="0")'
# s[0] = uuid
# s[1] = server
# s[2] = state
# s[3] = maxvalue
# s[4] = minvalue
sr_sems=${sr_sems:-()}

sr_statefile="${sr_statefile:-./client.state}"
sr_statevars="${sr_statevars:-sr_client sr_id sr_server sr_sems}"

##################################### global consts ####################################3

######################################### functions #######################################

_sr_curl() {
	local output ret
	echo "SEND: $2 <- \"$1\""  >&2
	output=$( #set -x; 
		curl -sS \
                -H "Content-Type: application/json" \
                -X POST -d "$1" http://"$2"/v1)
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error0" >&2 && return 1
    echo "RECV: $2 -> \"$output\"\n\n" >&2
    echo -ne "$output"
	return 0
}

_sr_send() {
	local server method params json output ret
	server=$1
	method=$2
	shift 2
	params=''
	if [ $# -eq 1 ]; then
		params="$1"
	else
		while [ $# -gt 3 ]; do
			params+="\"$1\":\"$2\","
			shift 2
		done
		if [ $# -gt 1 ]; then
	                params+="\"$1\":\"$2\""
	                shift 2
	        fi
	fi
	json="{\"id\":\"$sr_id\",\"method\":\"$method\",\"params\":[{$params}]}"
	sr_id="$((sr_id+1))"
	output="$( _sr_curl "$json" "$server" )"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error0" >&2 && return 1
	echo "$output" | jq '.' 2>/dev/null
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error1" >&2 && return 1
	_sr_check_error "$output"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error2" >&2 && return 1
	return 0
}

_sr_sems_iterators() {
	seq 0 $((${#sr_sems[@]}-1))
}

_sr_sems_getindex() {
	local s uuid="$1"
	for i in $(_sr_sems_iterators); do
		eval ${sr_sems[$i]}
		if [ "${s[0]}" = "$uuid" ]; then
			echo -ne "$i"
			return
		fi
	done
	echo -ne "0"
	return 0
}

_sr_sem_get() {
	local s uuid="$1"
	for i in $(_sr_sems_iterators); do
		eval ${sr_sems[$i]}
		if [ "${s[0]}" = "$uuid" ]; then
			typeset -p s
			return 0
		fi
	done
	return 1
}

_sr_sems_exists() {
	local s uuid="$1"
	for i in $(_sr_sems_iterators); do
		eval ${sr_sems[$i]}
		if [ "${s[0]}" = "$uuid" ]; then
			return 0
		fi
	done
	return 1
}

_sr_sems_add() {
	local s uuid="$1" 
	[ -z "$uuid" ] && echo "ERROR: $FUNCNAME no uuid given..." && return 1
	local server="$2" curval=${3:-0} minval=${4:-1} maxval=${5:-0}
	if ! _sr_sems_exists "$uuid"; then
		s=( "$uuid" "$server" "$curval" "$minval" "$maxval" )
		sr_sems+=( "$(typeset -p s)" )
		export sr_sems
	fi
	return 0
}

_sr_sems_rm() {
	local s uuid="$1" 
	[ -z "$uuid" ] && echo "ERROR: $FUNCNAME no uuid given..." && return 1
	local num sr_sems_new=()
	for i in $(_sr_sems_iterators); do
		eval ${sr_sems[$i]}
		if [ "${s[0]}" != "$uuid" ]; then
			sr_sems_new+=("${sr_sems[$i]}")
		fi
	done
	sr_sems=("${sr_sems_new[@]}")
	export sr_sems_new
	return 0
}

_sr_sems_incdec() {
	local s uuid="$2" 
	[ -z "$uuid" ] && echo "ERROR: $FUNCNAME no uuid given..." && return 1
	local DOING="$1"
	_sr_sems_add "$uuid"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	eval $(_sr_sem_get "$uuid")
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	s[2]=$[ ${s[2]} $DOING 1 ]
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	_sr_sems_rm  "$uuid"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	_sr_sems_add "${s[@]}"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	return 0
}

_sr_sems_inc() {
	_sr_sems_incdec "+" "$@"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	return 0
}
_sr_sems_dec() {
	_sr_sems_incdec "-" "$@" 
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	return 0
}

_sr_sems_print() {
	for i in $(_sr_sems_iterators); do 
		echo "\${sr_sems[$i]} = ${sr_sems[$i]}"
	done
	return 0
}

_sr_parse_uuid_param() {
	local s
	if [[ "$1" =~ - ]]; then
		echo -ne "$1"
	elif [ "$1" -eq "$1" ] 2>/dev/null; then
		eval ${sr_sems[$1]}
		echo ${s[0]}
	else
		echo "Unsupported uuid parameter"
		return 1
	fi
	return 0
}

check_port_is_open() {
	netstat -lnt | awk '$6 == "LISTEN" && $4 ~ ".:'"$1"'"' >/dev/null 2>/dev/null
}

_sr_load_state() {
	source "$sr_statefile" || return 1
	return 0
}

_sr_save_state() {
	typeset -p $sr_statevars > "$sr_statefile" || return 1
	return 0
}

_sr_lock() {
	exec 100>$sr_statefile
	flock -x 100 || return 1
	_sr_load_state || return 1
	return 0
}

_sr_unlock() {
	_sr_save_state || return 1
	exec 3>&-
	return 0
}

_sr_check_error() {
	local error="$(echo "$1" | jq -r .error)"
	[ "$error" != "null" ] && echo "ERROR: $FUNCNAME - $error" >&2 && return 1
	return 0
}

######################################### exported functions locked ############################################

_sr_Maintenance.Hello() {
	local Name="${1:-SR Kamil Cukrowski Bash Scripts!}"
	local server="${2:-$sr_server}"
	local method=$(echo "$FUNCNAME" | sed 's/_sr_//')
	_sr_send "$server" "$method" "Name" "$Name"
}

_sr_Locks.CreateSemaphore() {
	local method UUID server client output uuid CurrentVal MinVal MaxVal
	method=$(echo "$FUNCNAME" | sed 's/_sr_//')
	CurrentVal=${1:-1}
	MinVal=${2:-0}
	MaxVal=${3:-1}
	server="${4:-$sr_server}"
	output="$(_sr_send "$server" "$method" "\"CurrentVal\":$CurrentVal,\"MinVal\":$MinVal,\"MaxVal\":$MaxVal")"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	echo "$output"
	uuid="$(echo "$output" | jq -r .result.UUID )"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	_sr_sems_add "$uuid" "$server" "$CurrentVal" "$MinVal" "$MaxVal"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	echo "Created semaphore with UUID=\"$uuid\" or INDEX=$(_sr_sems_getindex "$uuid")"
}

_sr_Locks.DeleteSemaphore() {
	local method UUID server client output uuid
	method=$(echo "$FUNCNAME" | sed 's/_sr_//')
	if [ -z "$1" ]; then
		echo "USAGE: sr_Locks.DeleteSemaphore <UUID>"
		return 1
	fi
	UUID=$(_sr_parse_uuid_param $1)
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	server="${2:-$sr_server}"

	output="$(_sr_send "$server" "$method" UUID $UUID )"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	echo "$output"|jq
	uuid="$(echo "$output" | jq -r .result.UUID )"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	_sr_sems_rm "$uuid"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	echo "Deleted semaphore with UUID=\"$uuid\""
}

_sr_Locks.DecrementSemaphore() {
	local method UUID server client output uuid
	method=$(echo "$FUNCNAME" | sed 's/_sr_//')
	if [ -z "$1" ]; then
		echo "USAGE: sr_Locks.DeleteSemaphore <UUID>"
		return 1
	fi
	UUID=$(_sr_parse_uuid_param $1)
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	server="${2:-$sr_server}"
	client="${3:-$sr_client}"

	output="$(_sr_send "$server" "$method" UUID "$UUID" owner "$client" )"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	uuid="$(echo "$output" | jq -r .result.UUID )"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	_sr_sems_dec "$uuid"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	echo "Decremented semaphore with UUID=\"$uuid\""
}

_sr_Locks.IncrementSemaphore() {
	local method UUID server client output uuid
	method=$(echo "$FUNCNAME" | sed 's/_sr_//')
	if [ -z "$1" ]; then
		echo "USAGE: sr_Locks.DeleteSemaphore <UUID>"
		return 1
	fi
	UUID=$(_sr_parse_uuid_param $1)
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	server="${2:-$sr_server}"
	client="${3:-$sr_client}"

	output="$(_sr_send "$server" "$method" UUID "$UUID" owner "$client" )"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	uuid="$(echo "$output" | jq -r .result.UUID )"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	_sr_sems_inc "$uuid"
	[ $? -ne 0 ] && echo "ERROR: $FUNCNAME error" >&2 && return 1
	echo "Incremeneted semaphore with UUID=\"$uuid\""
}

_sr_Locks.Dump() {
	local method=$(echo "$FUNCNAME" | sed 's/_sr_//')
	local server="${1:-$sr_server}"

	_sr_send "$server" "$method"
}


########################################## exported functions ###############################

# too much bash-isms
for i in $( (declare -F | grep "declare -f _sr_Lock."; declare -F | grep "declare -f _sr_Maintenance.") | sed 's/declare -f //'); do
	eval "function $(echo -n "$i"|sed 's/_sr_/sr_/') { local ret=0; trap '_sr_unlock' SIGINT; _sr_lock || return 1; \"$i\" \"\$@\" || ret=$?; _sr_unlock || return 1; return $ret; }"
done

sr_start() {
	local mode=${1:-all}
	local client=${2:-127.0.0.1:7891}
	local server=${3:-127.0.0.1:7890}
	local clientip="$(echo "$client" | cut -d: -f1)"
	local clientport="$(echo "$client" | cut -d: -f2)"
	case "$mode" in
	clear)
		echo "export sr_id=1"
		export sr_id=1
		echo "export sr_server=\"$server\""
		export sr_server="$server"
		echo "export sr_sems=()"
		export sr_sems=()
		_sr_save_state
		;;
	startclient)
		if check_port_is_open "$port"; then
			echo "ERROR - runing client server, someone already listening to port $port."
			return 1;
		fi
		( set -x; ./run_client.sh $clientip $clientport 2>&1 >./logs_run_client.sh.txt) &
		sr_clientserver=$!
		echo "export sr_clientserver=$sr_clientserver"
		export sr_clientserver=$sr_clientserver
		;;
	*)
		"$FUNCNAME" clear "$2" "$3" && "$FUNCNAME" startclient "$2" "$3"
		;;
	esac
	echo
}

sr_stop() {
	killall tcpserver
}
