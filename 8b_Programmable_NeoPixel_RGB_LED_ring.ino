// Tutorial 8b. Programmable NeoPixel RGB LED ring

// Main parts: Adafruit Metro Mini, NeoPixel Ring 24 x 5050 RGB LED,
// CdS photoresistor, 10k trim potentiometer, momentary switch

// Library required to drive RGB LEDs; use the latest version
#include "FastLED.h"

// Variables that remain constant
const byte pinData = 4; // Digital output pin to LED ring
const byte pinSensor = A0; // Analog input pin from photoresistor
const byte pinPotentiometer = A1; // Analog input pin from trim potentiometer
const byte pinSwitch = A2; // Analog input pin from momentary switch
const byte numLeds = 24; // Number of LEDs
struct CRGB leds[numLeds]; // Declare an array that stores each LED's data

// Variables that can change
int sensorValue = 0; // Sets the LED ring's overall brightness
int sensorMin = 1023;
int sensorMax = 0;
byte lastSwitchState = HIGH; // Momentary switch assumed open at start
byte ledPos = 0; // Array position of LED that will be lit (0 to 23 = 24)
int displayDirection = 1; // 1 = clockwise, -1 = counter-clockwise

void setup()
{
  // Initialise the FastLED library with the type of programmable RGB LED
  // used, the digital output pin the LED ring is wired to, the array that
  // holds each LED's data, and the number of LEDs in the ring
  FastLED.addLeds<NEOPIXEL, pinData>(leds, numLeds);

  // Initialise the momentary switch pin with an internal pull-up resistor
  // so that the momentary switch is read as open (= HIGH) at start
  pinMode (pinSwitch, INPUT_PULLUP);

  // Initialise Metro Mini's built-in red LED pin
  pinMode(13, OUTPUT);

  // Signal start of the calibration period with the built-in red LED
  digitalWrite(13, HIGH);

  // Read the voltage from the photoresistor's pin for five seconds. Cover
  // the photoresistor and then remove the cover to expose it to the ambient
  // light. This will set the minimum and maximum brightness levels
  while (millis() < 5000)
  {
    sensorValue = analogRead(pinSensor);

    // Set the maximum sensor value
    if (sensorValue > sensorMax)
    {
      sensorMax = sensorValue;
    }

    // Set the minimum sensor value
    if (sensorValue < sensorMin)
    {
      sensorMin = sensorValue;
    }
  }

  // Signal end of the calibration period
  digitalWrite(13, LOW);
}

void loop()
{
  // Read the voltage from the potentiometer pin and scale it into a
  // 0 - 255 value range (= once around the full colour spectrum)
  int changeHue = map(analogRead(pinPotentiometer), 0, 1023, 0, 255);

  // A call to this function automatically sets the LED ring's overall
  // brightness continuously (once every loop() iteration)
  autoDim();

  // The momentary switch is hardware debounced with a 1uF capacitor; no
  // debouncing code is necessary. See http://www.gammon.com.au/switches
  // Read the voltage from the momentary switch pin to see if something
  // has changed (was the button pressed or released?)
  byte switchState = digitalRead (pinSwitch);

  // Has the momentary switch state changed since the last time it was
  // checked (once every loop() iteration)?
  if (switchState != lastSwitchState)
  {
    // First, store the current switch state for the next time around
    lastSwitchState = switchState;

    // Next, test if the switch was closed (button pressed)
    if (switchState == LOW)

    { // If so, toggle the display from clockwise to counter-clockwise
      displayDirection = -displayDirection;
    }
    // One could also add an else statement here, if one would want to
    // trigger something when the button was released (switch = HIGH)
  }

  // EVERY_N_MILLISECONDS, part of the FastLED library, avoids using
  // delay(), which stops program execution, which means that button
  // presses, sensor readings, etc. would not be detected! Always try
  // to avoid delay() and use millis()(or EVERY_N_MILLISECONDS when
  // using the FastLED library)
  EVERY_N_MILLISECONDS(25)
  {
    // Dims the LEDs by 64/256 (1/4) and thus sets the trail's length
    fadeToBlackBy(leds, numLeds, 64);

    // Sets the LED's hue according to the potentiometer rotation
    leds[ledPos] = CHSV(changeHue, 255, 255);

    // Shifts all LEDs one step in the currently active direction
    ledPos = ledPos + displayDirection;

    // If one end is reached, reset the position to loop around
    if (ledPos == numLeds)
    {
      ledPos = 0;
    }

    // If the other end is reached, reset the position to loop around
    else if (ledPos == 255)
    {
      ledPos = numLeds;
    }

    // Finally, display all LED's data (= illuminate the LED ring)
    FastLED.show();
  }
}

void autoDim()
{
  // Read the voltage from the photoresistor pin
  sensorValue = analogRead(pinSensor);

  // Scale the reading into the minimum and maximum values
  sensorValue = map(sensorValue, sensorMin, sensorMax, 0, 255);

  // Clip out-of-range readings
  sensorValue = constrain(sensorValue, 0, 255);

  // Set the LED ring's overall brightness
  FastLED.setBrightness(sensorValue);
}
