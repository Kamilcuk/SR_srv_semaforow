#!/bin/bash -xe

sudo systemctl is-active docker || sudo systemctl start docker
if [ ! -d kosiak ]; then
	git clone https://github.com/m-kostrzewa/SR-semaphore-server kosiak
	pushd kosiak
	        docker build -t kosiakserver -f Dockerfile.server .
		docker build -t kosiakclient -f Dockerfile.client 
		popd
fi


mode=$1
port=$2
shift
shift
case "$mode" in
server)
	docker run -it --rm --name kosiekserverrun -p "$port:7890" kosiakserver "$@"
	;;
client)
	docker run -it --rm --name kosiakclientrun -p "$port:7891" kosiakclient "$@"
	;;
esac


