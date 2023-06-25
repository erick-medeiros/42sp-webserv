FROM ubuntu:latest

ARG UID GID

RUN apt-get update && apt-get install -y \
	make g++ valgrind clang-format bash-completion pip \
	&& apt-get clean

RUN pip install requests

RUN echo "PS1='\w > '" >> /etc/bash.bashrc

RUN addgroup --gid $GID container
RUN adduser -u $UID -G container -h /home/container -D container
 
USER container

WORKDIR /webserv

EXPOSE 8080

CMD ["tail", "-f"]