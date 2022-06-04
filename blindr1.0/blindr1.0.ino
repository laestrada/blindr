/*
  Blindr1.0

  This sketch contains the code to operate window blinds over a bluetooth connection.

  The circuit:
  - Arduino Nano 33 IoT, A4988 Stepper Motor Driver, Nema17 Stepper Motor 17HS4023, and
    12V power supply

  You can use a generic Bluetooth® Low Energy central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>

// set up bluetooth
BLEService bluetoothService("180A");                                    // BLE service
BLEByteCharacteristic switchCharacteristic("2A57", BLERead | BLEWrite); // characteristic Digital output

// set motor driver pins
const int stepPin = 11;
const int dirPin = 12;
const int enablePin = 10;
const int lightPin = LED_BUILTIN; // use analog pin 0 for light sensor

boolean onTime = false;  // a variable that we will use to determine if we want to activate the curtain based on a time
boolean onLight = false; // a variable that we will use to determine if we want to activate the curtain based on the light level
boolean state;           // keep track if we are open or closed

int openHour, openMinute, closeHour, closeMinute = 0;
int nowHour, nowMinute;

void setup()
{
  Serial.println("setting up");
  Serial.begin(9600);
//  while (!Serial)
//    ;

  // begin initialization
  if (!BLE.begin())
  {
    Serial.println("Starting Bluetooth® Low Energy module failed!");

    while (1)
      ;
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Blindr1.0");
  BLE.setAdvertisedService(bluetoothService);

  // set various pins
  pinMode(enablePin, OUTPUT); // We set the enable pin to be an output
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  digitalWrite(enablePin, HIGH); // set enable pin high to disable the driver.
  digitalWrite(stepPin, LOW);    // set step pin low

  // add the characteristic to the service
  bluetoothService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(bluetoothService);
  
  // set the initial value for the characeristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();
  Serial.println("finished setting up");

}

void loop()
{
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();
  Serial.println("listening for bluetooth connection");

  if (central)
  {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    while (central.connected())
    {
      // if the remote device wrote to the characteristic,
      // use the value to control the blinds:
      if (switchCharacteristic.written())
      {
        switch (switchCharacteristic.value())
        {
        case 01:
          openCurtain(1);
          break;
        case 02:
          closeCurtain(1);
          break;
        case 03:
          closeCurtain(10);
          break;
        case 04:
          openCurtain(10);
          break;
        case 05:
          setClock();
          break;
        default:
          Serial.println("Unrecognized command");
          break;
        }
        if (onTime)
        { // check time to see if we need to open or close
          readTime();
        }
      }
    }
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
void openCurtain(int delay)
{
  Serial.println("opening curtain");
  digitalWrite(enablePin, LOW);
  delayMicroseconds(2);
  digitalWrite(dirPin, LOW);
  for (int i = 0; i <= (100 * 50) / delay; i++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1100*delay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2*delay);
  }
  digitalWrite(enablePin, HIGH);
  delayMicroseconds(2);
  Serial.println("curtain opened");
}
void closeCurtain(int delay)
{
  Serial.println("closing curtain");
  digitalWrite(enablePin, LOW);
  delayMicroseconds(2);
  digitalWrite(dirPin, HIGH);
  for (int i = 0; i <= (100 * 50) / delay; i++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1100*delay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2*delay);
  }
  digitalWrite(enablePin, HIGH);
  delayMicroseconds(2);
  Serial.println("curtain closed");
}

void setClock()
{
  //  char set = BLE.read();
  //  if(set == 'o'){  //set the open curtain time
  //    openHour = BLE.read();
  //    openMinute = BLE.read();
  //  }
  //  else if(set == 'c'){  //set the close curtain time
  //    closeHour = BLE.read();
  //    closeMinute = BLE.read();
  //  }
  //  else if(set == 's'){  //set the clock time
  //    int hour = BLE.read();
  //    int minute = BLE.read();
  //  }
}
void readTime()
{ // read the current time from the RTC
}
