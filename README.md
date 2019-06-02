# LoRa Motion sensor
These are the kicad PCB and software files for the LoRa motion sensor, a wireless proximity alarm using the RCWL-0516 sensor and SX1278 radio. PCB designed in Kicad 5.1. The cache should contain all custom footprints. Handheld receiver design coming soon.

![sensorrender](/hardware/LORA_motionsensor.png)

### Dependencies
Only dependency is the LoRa radio library by Sandeep Mistry, included in _/firmware/lib/_

### Software usage
Software was created and compiled with PlatformIO, but is usable by any arduino compatible IDE by compiling the program located in _/firmware/src_ as an arduino pro mini 3.3v/8mhz. Upload is done over ISP header. Be careful that programming signals are 3.3 volts, as the LoRa radio is not 5v tolerant.

### Using the device
Simply turn it on and place in an area you'd like to monitor. Choke points are best, such as near a gate, pathway, doorway or vehicle. The sensor works by detecting a phase change in it's 3Ghz oscillator and waking up the processor and radio, then sending the packet 'MOT' via LoRa radio protocol, along with packet and radio ID number.

Sensor range is about 20-30ft out the front and back, and less to the sides. Range seems somewhat dependent on size and speed of the object (dog, man, car, etc.), and whether or not the object is moving across or toward/away from the sensor. Sensor is immune to changes in heat or light, and can 'see' through non-conductive objects. It detects movement of conductive objects, be they people, animal or metal vehicle, and sometimes large amounts of tree leaves. Beware if placing near large leafy things in the wind.

The best documentation on the function and use of this sensor can be found here: (https://github.com/jdesbonnet/RCWL-0516)

The only critical part of placing it is near metal or another sensor. Any metal closer than about 2" to the sensor will cause it to go off constantly, and the sensors will interfere with each other closer than about 10feet(~3meters)

Testing is still ongoing, but the device is mostly feature complete. Sleep modes on the radio and ATmega are getting about two weeks (depending on number of wakeups) on a single 18650 battery.  

### Still todo

- [ ] program CRC and collision detection functions
- [ ] update and test boards with low-battery alert functions
- [x] upload platformio files
- [x] upload .pdf of schematic
- [ ] generate and upload a proper BOM

