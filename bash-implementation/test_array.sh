#!/bin/bash

s=( "llalaal" "alala" )
str="$(typeset -p s)"
array=( ""  "declare -a \"llalaal\" []\"alala\"" "pippo" )
delete=(    "declare -a \"llalaal\" []\"alala\"" )

set -x
echo "${array[@]}" >/dev/null
array=(  "${array[@]/$delete}" )
echo "${array[@]}" >/dev/null

