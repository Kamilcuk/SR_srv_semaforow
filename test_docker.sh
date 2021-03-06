#!/bin/bash
getipaddr() {
docker inspect ${1} | jq -r '.[0].NetworkSettings.IPAddress'
}

docker pull kamilcuk/sr_srv_semaforow
docker pull kosiak/sr-server
docker pull kosiak/sr-client

# server kamil
docker run -it --rm --name kamilserver --net=host kamilcuk/sr_srv_semaforow /usr/bin/SR_srv_semaforow --type server --listenport 7889

#server kosiak
docker run -it --rm --name kosiakserver --net=host kosiak/sr-server

#client kamil
docker run -it --rm --name kamilclient  --net=host kamilcuk/sr_srv_semaforow /usr/bin/SR_srv_semaforow --type client --listenport 7891 --listenip 127.0.0.1 --serverurl $(getipaddr kamilserver):7889
docker run -it --rm --name kamilclient  --net=host kamilcuk/sr_srv_semaforow /usr/bin/SR_srv_semaforow --type client --listenport 7892 --listenip 127.0.0.1 --serverurl <TUTAJ KOSIAKA IP>:7890
# server kosiaka potrafi sam znajdować sobie swoje ip i na nim nasłuchiwać

#client kosiak
docker run -it --rm --name kosiakclient --net=host kosiak/sr-client


# test Deadlock detection on kamil's clients
## run server on a port in another console, and then run:
/bin/bash -c 'docker run -a stdin -a stdout -i -t --rm --name kamilclient3 --net=host -v $(pwd):/SR_srv_semaforow kamilcuk/sr_srv_semaforow /bin/bash -c "cd /SR_srv_semaforow; ./test_server.sh client_mode2 <server>:<port>"'

