Extras
-----------------------------------------------

These are extras that were made during the development of SensorNet, mostly out of necessity.

The Temp Control folder contains a program to self regulate the RPis temperature using a fan. The fan speed is regulated using a PWM signal.

The EAGLE folder contains a PCB design for an interface board for the nRF24L01+. The board includes an optional 3.33v regulator, power LED, and reservoir capacitors. The reservoir capacitors are necessary when using a small microcontroller and a nRF24L01+ with a LNA. The power draw when switching to TX mode may be too much for the microcontroller. Reservoir capacitors fix this problem in most cases.