#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>

#include <SPI.h>
#include <LoRa.h>

#define csPin A2
#define resetPin A1
#define irqPin 3 //this is interrupt 1, for the radio receiver

#define led 8
#define button A3

// These are the jumper pins for address selecting
#define jumpone 7
#define jumptwo 5
#define jumpthree 6

int chrg_lvl = A0;

int localAddress;       //the radio address to be set by a function that reads jumpers or dip switches
int motionPin = 2;      // this is interrupt 0, for the motion sensor. It is pulled low by 100k
int wakedelay = 4000;   //stay awake for 4 seconds before re-enabling interrupt and going back to sleep
int counter = 0;
byte destination = 0x25;

String const motAlert = "MOT";

int volatile battVolt; //ADC reading of battery voltage, through a 100k-100k voltage divider
bool volatile battlow = false; // a variable to pass to the motion alert interrupt. It will append a message that the battery is low if it is below 10%

void getaddr()           //read dip switches to determine device address on startup
  {
  //byte jumperArray[3];
  localAddress = 0x28;   //temporarily hard coding this for testing
  }


void errorBlink() //blink the led if the radio hangs. LoRa.endPacket() returns 1 if successful, 0 if error.
{
  for (int i = 0; i<21 ; i++)
  {
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(100);
  }
}

void wakeUp() // ISR to send message when motion detected
{
  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(motionPin));
}

void motion_alert() //Send the motion detected message.
  {
    // send packet
    LoRa.beginPacket();                   // start packet
    LoRa.write(destination);              // add destination address
    LoRa.write(localAddress);             // add sender address
    LoRa.write(1);                        // add message ID
    LoRa.write(motAlert.length());        // add payload length
    LoRa.print(motAlert);                 // add payload
    if (!LoRa.endPacket())
    {
      errorBlink();
    }

  }


void sleepNow()
  {
   //Serial.println("going to sleep.");
   set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
   noInterrupts();
   sleep_enable();          // enables the sleep bit in the mcucr register so sleep is possible. just a safety pin
   attachInterrupt(digitalPinToInterrupt(motionPin), wakeUp, RISING);
   delay(10);
   interrupts();
   sleep_cpu(); // trying sleep_cpu instead of sleep_mode
   //sleep_mode();            // here the device is actually put to sleep.
  }

void batteryvoltage() // get a read on the battery level, convert to percentage and set batt_low flag if it is below 10%
{
  //Serial.println("Battery Voltage function");
  battVolt = analogRead(A0) * 2; //battery level as read through the 100k-100k voltage divider
  //Serial.print("Divider is: "); Serial.println(divider);
  if (battVolt <= 800) {battlow = true;} // Low battery will probably be around 3.1 volts, but we'll set this flag when the battery is just slightly drained for testing purposes
  else {battlow = false;}
}

 void batteryAlert()
{
    String battMessage = "BATT: "  +  (String)battVolt;
    LoRa.beginPacket();                   // start packet
    LoRa.write(destination);              // add destination address
    LoRa.write(localAddress);             // add sender address
    LoRa.write(9);                        // add message ID
    LoRa.write(battMessage.length());     // add payload length
    LoRa.print(battMessage);              // add payload
    if (!LoRa.endPacket())
    {
      errorBlink();
    }                                     // finish packet and send it
}



void setup() {
  //Serial.begin(9600);
  //Serial.println("starting program");
  LoRa.setPins(csPin, resetPin, irqPin);

  pinMode(chrg_lvl, INPUT);
  pinMode(led, OUTPUT);

  for (int i = 0; i<9; i++) //blink led on startup
  {
    digitalWrite(led, HIGH);
    delay(50);
    digitalWrite(led, LOW);
    delay(50);
  }

  //pinMode(jumpone, INPUT_PULLUP);
  //pinMode(jumptwo, INPUT_PULLUP);
  //pinMode(jumpthree, INPUT_PULLUP);
  getaddr(); //get the address set for the LORA radio

  pinMode(motionPin, INPUT); //has external pulldown resistor

  if (!LoRa.begin(430E6)) {
    errorBlink();
  }
  batteryvoltage();
  batteryAlert();
  delay(8000);   //Wait a bit after startup for user to move out of the way

}


void loop() {
  delay(100);
  sleepNow(); // no more motion detected. Going to sleep
  //delay(100);
  while (digitalRead(motionPin) == HIGH) //after waking up and sending the motion packet, check again for motion. If the sensor pin is still high, don't go back to sleep- send another packet!
  {
    digitalWrite(led, HIGH);            //you can turn on the LED here if you wish for testing the motion sensor.
    motion_alert();
    delay(wakedelay);
    digitalWrite(led, LOW);
  }
  //batteryvoltage();
  //if (battlow == true){batteryAlert();}
}
