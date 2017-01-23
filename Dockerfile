#FROM base/archlinux
FROM finalduty/archlinux:daily
MAINTAINER Kamil Cukrowski <kamilcukrowski@gmail.com>
RUN pacman -Suy --needed --noconfirm --noprogressbar sudo base-devel git boost cmake qt5-base && \
	sed -e "/nice/s/\*/#*/" -i /etc/security/limits.conf && \
	echo "nobody ALL=(ALL:ALL) NOPASSWD: ALL" | (VISUAL="tee -a" EDITOR="tee -a" visudo) && \
	cd /tmp && git clone https://aur.archlinux.org/cpp-netlib.git cpp-netlib && \
	cd ./cpp-netlib && git reset --hard e15211bf5b909285784dd5b2de48e53bbb4ae9d1 && \
	chown nobody:nobody /tmp/cpp-netlib && \
	sudo -u nobody makepkg && pacman -U --noconfirm cpp-netlib-*.pkg.tar.xz
RUN /project/SR_srv_semaforow
ADD . /project
RUN cd /project && qmake && make -j4

