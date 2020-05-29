DOCKERUN_TINYGO    := docker run --rm -ti -v $(PWD):/go/src --privileged -v /dev/ttyACM0:/dev/ttyACM0 --workdir /go/src inosion/tinygo-dev-extra
DOCKER_ARDUINO_CLI := docker run --rm -ti -v $(PWD):/v --workdir /v arduino/arduino-cli

all: compile

compile: blinky.go
	$(DOCKERUN) tinygo build -target arduino -o build/blinky.hex blinky.go 

flash: build/blinky.hex
	# $(DOCKERUN) bash -c "stty -F /dev/ttyACM0 ispeed 1200 ospeed 1200 && avrdude -C/etc/avrdude.conf -patmega32u4 -cavr109 -v -v -v -v -P/dev/ttyACM0 -b57600 -D -Uflash:w:build/blinky.hex:i"
	# python reset-leonardo.py /dev/ttyACM0
	# sleep 2
	# $(DOCKERUN) 
	# avrdude -patmega32u4 -cavr109 -v -v -v -v -P/dev/ttyACM0 -b57600 -D -Uflash:w:build/blinky.hex:i
	avrdude avrdude -p m2560 -c stk500v2 -vvv -P /dev/ttyACM0 -b115200 -D -F -U flash:w:build/blinky.hex:i
# flash -target arduino build/blinky.hex
# avrdude -c arduino -p atmega328p -P /dev/ttyACM0 -U flash:w:build/arduino-colorlamp.hex

flash-mega:
	$(DOCKERUN) tinygo flash -target arduino-mega2560 blinky.go

flash-uno:
	$(DOCKERUN) tinygo flash -target arduino blinky.go	

flash-leo:
	# python reset-leonardo.py /dev/ttyACM0
	# sleep 1
	# $(DOCKERUN) tinygo flash -target arduino blinky.go
	$(DOCKERUNR) avrdude -patmega32u4 -cavr109 -vvvv -P /dev/ttyACM0 -b 57600 -D -Uflash:w:build/blinky.hex:i