package main

import (
    "machine"
    "time"
)


func main() {
    led := machine.P17 // LED
	led.Configure(machine.PinConfig{Mode: machine.PinOutput})

    for {

        led.Low()
        time.Sleep(time.Millisecond * 1000)

        led.High()
		time.Sleep(time.Millisecond * 1000)
		
    }
}