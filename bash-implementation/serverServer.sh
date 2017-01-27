#!/bin/bash

##################### functions ###########################3

parse_input() {
	id="$(echo "$1" | jq -r '.id')"
	method="$(echo "$1" | jq -r '.method')"
	params[all]="$(echo "$1" | jq -r '.params' | head -n -1 | tail -n +2)"
	while read line; do 
		name="$(echo "$line" | cut -d\" -f2)"
		value="$(echo "$line" | cut -d\" -f4)"
		if [[ ! "$name" =~ ^[a-zA-Z]$ || ! "$value" =~ ^[a-zA-Z]$ ]]; then
			continue;
		fi
		eval "params[$name]=$value"
	done <<<"$(echo "${params[all]}")"
}
output_json() {
	local error="${1:-}"
	shift
	local result=""
	while [ $# -gt 3 ]; do
		result+="\"$1\":\"$2\","
		shift 2
	done
	while [ $# -gt 1 ]; do
		result+="\"$1\":\"$2\""
		shift 2
	done
	if [ -z $error ]; then
		error="null"
	else
		error="\"$erro\""
	fi
	# method and id passed as globals
	echo "{\"id\":\"$id\",\"error\":$error,\"result\":{$result}}"
}
exit_print_output() {
	local output="$1"
	echo -n "Content-Length: ${#output}

$output"
	echo "SEND: -> $output" >&2
	exit 0
}

####################################### main ###################################

# handle http read
raw_input=$(while IFS='' read -r -t 0.001 -N1 line; do echo -ne "$line"; done; )
json_input=$(echo "$raw_input"|tail -n1)
json="$json_input"
output=''
echo 'HTTP/1.1 200 OK
Content-Type: application/json; charset=utf-8
Date: '"$(date +"%a, %d %b %Y %H:%M:%S %Z")"

echo "RECV: <- $json_input" >&2
parse_input $json_input
case "$method" in
Maintenance.Hello)
	output+="$(output_json "" "Message" "Hello, ${params[Name]}")"
	;;
Maintenance.Heartbeat)
	output+="$(output_json "" "Message")"
	;;

esac

exit_print_output "$output"
