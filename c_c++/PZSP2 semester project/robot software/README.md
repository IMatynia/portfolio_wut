# Cel projektu

Rozwinięcie oprogramowania do robota mapującego oraz realizacja oprogramowania do wizualizacji, w
celu skanowania i reprezentacji pomieszczeń.

# Building software

* Download RIOT OS code - `git clone https://github.com/RIOT-OS/RIOT`
* Download robot software
* Update in robot software `Makefile` RIOT OS directory - update `RIOTBASE` variable
* From the robot software main directory execute command `make`

# Flashing nucleo boards without additional programmer

Most Nucleo boards contains ST-LINK programmer. To program board you could 
simple copy hex file into flash.

In RIOT to do this automatically use command:

`make BOARD=.... PROGRAMMER=cpy2remed flash`

# Firmware flashing procedure

* Turn off robot - use switch between battery-pack and nucleo board
* Move jumper JP5 from external power to power from ST-Link USB connection
 (JP5 to U5V - pins 1 and 2)
* Connect nucleo board to computer using USB
* Connect device to virtual machine - use Menu \ Devices \ USB \ STMicroelectronics STM32 STLink
* Wait for removable media
* Use command `make flash` - full version of command `make BOARD=nucleo-f334r8 PROGRAMMER=cpy2remed flash`
* Disconect USB cable
* Move jumper JP5 from power from ST-Link USB connection to external power
 (JP5 to E5V - pins 2 and 3)
* Turn external power on - use switch between battery-pack and nucleo board

# Robot2 sensors - detailes concerning connections 

## Ultrasonic sensors

right sensor purple/yellow (PB7/PA0)
middle sensor  white/orange (PH0/PA1)
