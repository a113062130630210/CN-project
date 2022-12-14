FROM ubuntu:bionic
FROM gcc:latest

ADD . /usr/src
WORKDIR /usr/src

RUN make

CMD ["./server"]
# ENTRYPOINT [ "./server" ]
