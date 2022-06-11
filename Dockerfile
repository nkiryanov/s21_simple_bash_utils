FROM ubuntu:latest
RUN apt-get update && \
    apt-get install -y \
    git gcc make pkg-config fish check valgrind
WORKDIR /code/src
CMD fish
