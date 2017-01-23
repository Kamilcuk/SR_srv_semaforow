# SR_srv_semaforow
Projekt na SR

# how to

## sync

```
git clone --recursive https://github.com/Kamilcuk/SR_srv_semaforow
```

## docker

Zbudowany na docker hub na https://hub.docker.com/r/kamilcuk/sr_srv_semaforow/  

### server

```
docker pull kamilcuk/sr_srv_semaforow
docker run -it -rm -name ksemaserver kamilcuk/sr_srv_semaforow --type server
```

### client

```
docker pull kamilcuk/sr_srv_semaforow
docker run -it -rm -name ksemaserver kamilcuk/sr_srv_semaforow --type client --serverurl 'http://123.123.123.123:8080/v1'
```

### help

```
docker run -it -rm -name ksemaserver kamilcuk/sr_srv_semaforow --help
```

# Zadanie 3 

```
Zaimplementować serwer semaforów udostępniający podstawowe operacje na semaforach (tworzenie, operacja p, operacja v, usuwanie) oraz API dostępu do usług serwera. API wykonać tak aby wykorzystując algorytm Chandy-Misra-Haasa [1] realizowały rozproszone wykrywanie blokad. Wykrycie blokady powinno skutkować zgłoszeniem wyjątku.
Zasady implementacji i oceniania
Projekty przeznaczone są dla zespołów 4-osobowych. Każdy osoba implementuje zadanie w innym języku programowana/środowisku. Przez różne środowiska rozumiane są: C++ na platformie Windows, Java, C#, C++ na platformie Unix, Python i inne po uzgodnieniu z prowadzącym.
Oceniane są poszczególne osoby, ale bardzo silny wpływ na ocenę ma umiejętność współpracy z pozostałymi osobami. W na ocenę mają wpływ następujące czynniki:
Jakość implementacji
Liczba innych implementacji, z którymi dana implementacja współpracuje.
Zasady są następujące:
Jeżeli dana osoba zrealizuje zadanie w 100%, ale nie uda się go zintegrować z żadną inną implementacją otrzymuje12 p.
Integracja z 1 implementacją dodaje 4 p.
Integracja z 2 implementacjami dodaje 8 p.
Integracja z większą liczbą implementacji niż 2 nie zwiększa dalej punktacji.
Usterki w rozwiązaniu obniżają punktację odpowiednio do ich skali.
```

# Dokumentacja
w google docu cholera wie gdzie

# Autor
Kamil Cukrowski
