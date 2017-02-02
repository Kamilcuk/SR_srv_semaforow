#!/bin/bash
getipaddr() {
docker inspect ${1} | jq -r '.[0].NetworkSettings.IPAddress'
}

docker pull kamilcuk/sr_srv_semaforow
docker pull kamilcuk/sr_srv_semaforow:kosiak_server
docker pull kamilcuk/sr_srv_semaforow:kosiak_client

# server kamil
docker run -it --rm --name kamilserver --net=host kamilcuk/sr_srv_semaforow /usr/bin/SR_srv_semaforow --type server --listenport 7889

#server kosiak
docker run -it --rm --name kosiakserver --net=host kamilcuk/sr_srv_semaforow:kosiak_server

#client kamil
docker run -it --rm --name kamilclient  --net=host kamilcuk/sr_srv_semaforow /usr/bin/SR_srv_semaforow --type client --listenport 7891 --listenip 127.0.0.1 --serverurl $(getipaddr kamilserver):7889
docker run -it --rm --name kamilclient  --net=host kamilcuk/sr_srv_semaforow /usr/bin/SR_srv_semaforow --type client --listenport 7892 --listenip 127.0.0.1 --serverurl <TUTAJ KOSIAKA IP>:7890
# server kosiaka potrafi sam znajdować sobie swoje ip i na nim nasłuchiwać

#client kosiak
docker run -it --rm --name kosiakclient --net=host kamilcuk/sr_srv_semaforow:kosiak_client


# test Deadlock detection on kamil's clients
## run server on a port in another console, and then run:
docker run -it --rm --name kamilclient   --net=host kamilcuk/sr_srv_semaforow /SR_srv_semaforow/test_server.sh client_mode2 <server_addr>:<server_port>

