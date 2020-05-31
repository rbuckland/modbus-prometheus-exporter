FROM ubuntu:latest

RUN apt-get update 
RUN apt-get install -y \
    avrdude \
    make \
    python \
    python-pip \
    gcc-avr \
    curl \
    git \
    arduino-mk \
    && rm -rf /var/lib/apt/lists/*

RUN cd /usr/local && git clone https://github.com/sudar/Arduino-Makefile 
RUN pip install pyserial