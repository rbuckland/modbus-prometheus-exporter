include tooling.makefile

IMAGE=inosion/arduino-build
DOCKER_OPTS := -v /dev:/dev --privileged # --user $$(id -u):$$(id -g)

TARGET = modbus-prometheus-exporter
ARDUINO_DIR  = /usr/share/arduino
AVR_TOOLS_PATH = /usr/bin

#
# setup project local stuff
#
ARDUINO_LIBS = protothreads-arduino SPI Ethernet
BOARD_TAG    = uno  # for mega use mega2560
ARDUINO_PORT = /dev/ttyACM0  # change this to the port used by your board
ARDUINO_SKETCHBOOK = $(realpath .)

inosion/arduino-build:
ifneq ($(IN_NESTED_DOCKER),yes)
	docker build -t $(IMAGE) .
endif

##RUN_IN_IMAGE: inosion/arduino-build
code-upload: docker-nested-run
ifeq ($(IN_NESTED_DOCKER),yes)
	device=$(shell ls  /dev/ttyACM* | tail)
	ls -al /dev/ttyA*
	@echo "uploading to $(device)"
	$(MAKE) arduino=$(device) upload
endif

##RUN_IN_IMAGE: inosion/arduino-build
echo: docker-nested-run
ifeq ($(IN_NESTED_DOCKER),yes)
	@echo hello
endif

##RUN_IN_IMAGE: inosion/arduino-build
clean:: docker-nested-run 

##RUN_IN_IMAGE: inosion/arduino-build
do-compile: | docker-nested-run compile

-include /usr/local/Arduino-Makefile/Arduino.mk
