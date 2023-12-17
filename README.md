<img src="images/IMG_1197.jpeg" alt="Plant Health Probe" width="600">

# Plant Health Probe
A personal project by Michael Hogue.

## Introduction
This repository contains all of the source code (and gerber files for the PCB) written for a system which monitors plant health-affecting conditions via 3 on-board sensors. The system monitors soil moisture, light intensity, and temperature. A re-purposed Nokia 5110 LCD is used to output details on conditions sampled by the sensors. A button is included on the board to cycle through 3 views. The first view shows 2 percentage bars for the currrent amount of soil moisture and light intensity. The second and third views show more details about the soil moisture and light intensity, respectively.   

I started this project in order to introduce myself to building software for embedded systems. My goal was to create something that was relatively simple to complete, but was enough to challenge me in an area I haven't worked in before. The challenge was to write a bare-metal program in C that would be efficient and achieve the goal I had in mind for the end product. I chose the RP2040 microcontroller on the Raspberry Pi Pico development board for this project as there is a great C SDK that provides many useful tools, while still staying close to the metal.

## Demo
(Click to watch)   
[![Demo Video](https://img.youtube.com/vi/UA-I9vfqD_w/0.jpg)](https://youtu.be/UA-I9vfqD_w)

## Hardware Components
- [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/)
- [Nokia 5110 LCD](https://www.sparkfun.com/products/10168)
- [DS18B20 Digital Temperature Sensor + 4.7k resistor](https://www.adafruit.com/product/374)
- [BH1750 Ambient Light Sensor](https://www.adafruit.com/product/4681)
- [Capacitive Moisture Sensor](https://www.adafruit.com/product/4026)
- [2x AA Battery Pack](https://www.amazon.com/dp/B09V7Z4MT7?psc=1&ref=ppx_yo2ov_dt_b_product_details)
- Push Button
- Power Switch
- PCB manufactured by [JLCPCB](https://jlcpcb.com)

## Challenges I Faced
One major hurdle I had to overcome was dealing with hardware. I had never before read a datasheet or designed a PCB and I had only ever soldered a few times. However, in order to communicate with the sensors and the LCD, studying the datasheet was necessary. In particular, the datasheet was extremely important when writing the driver to communicate with the DS18B20 temperature sensor as I not only needed to know which commands to send it, but also the timing of communication over the 1-wire bus. The RP2040 also does not contain any dedicated hardware for the 1-wire protocol, so I had to program one of the PIO state machines on the MCU using PIO assembly. This was a much more elegant solution than bit-banging the temperature sensor.

In addition, I made a couple of mistakes when designing the PCB.    
First, I drew a trace from the LCD's SPI enable pin to the incorrect pin on the Pico. The pico has dedicated pins for each SPI function which route to one of the on-chip SPI blocks. The solution to this problem was simply to solder a wire connecting the incorrect pin to the correct pin on the back of the PCB.   
Second, I incorrectly wired the 1-wire bus for the temperature sensor. The 1-wire bus requires a 4.7k pull-up resistor connected to VDD. However, in the PCB design, I have the resistor directly wired between the temperature sensor and the Pico. The simple solution to this design error was to use the RP2040's internal pull-up on the 1-wire data line.

## What Could Be Improved
- Device could include more sensors, such as a soil pH sensor.
- New PCB design could fix the issues mentioned above.
- My soldering...

## Contributing
I strongly encourage anyone to open an issue if you spot anything in the code which I could have done better. I am open to all constructive criticism!

## Credit
Inspiration for using the on-chip PIO for the DS18B20 driver came from:  
https://www.i-programmer.info/programming/hardware/14527-the-pico-in-c-a-1-wire-pio-program.html?start=1

ALL other code written for this project was done only by me.
