#include <Encoder.h>
#include <FastLED.h>

const int ledPin = 13; // Onboard LED
const int ledStripPin = 6; // Digital pin for the LED strip
const int encoderPinA = 2;
const int encoderPinB = 3;
const int numLeds = 10; // Number of LEDs in the WS2812B strip
const int brightness = 50; // LED brightness (0-255)

unsigned long currentMillis = 0;
unsigned long stripPreviousMillis = 0;
int stripBlinkDuration = 0;
CRGB leds[numLeds];

Encoder myEncoder(encoderPinA, encoderPinB);
long encoderValue = 0;

// Variables for blinking the LED strip:
int blink_enabled = 0;
unsigned long previousMillis = 0;
const int blinkDuration = 25;
const int blinkDelays[] = {25, 30, 50, 75, 100, 150, 200, 300, 500, 1000}; // 10 distinct steps
int currentBlinkDelayIndex = 9; // Start with longer delay

int pulseCounter = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(ledStripPin, OUTPUT); // Set the LED strip pin as an output
  FastLED.addLeds<WS2812, ledStripPin, GRB>(leds, numLeds); // Define LED strip type and data pin
  FastLED.setBrightness(brightness); // Set LED brightness
  
  Serial.begin(9600);
}

void loop() {
  long newEncoderValue = myEncoder.read();

  if (newEncoderValue > encoderValue) {
    pulseCounter++;

    if (pulseCounter == 4) {
      increaseBlinkDelay();
      printBlinkDelay();
      setLEDColor(); // Set LED color based on blink delay
      pulseCounter = 0;
    }
  } else if (newEncoderValue < encoderValue) {
    pulseCounter++;

    if (pulseCounter == 4) {
      decreaseBlinkDelay();
      printBlinkDelay();
      setLEDColor(); // Set LED strip color based on blink delay
      pulseCounter = 0;
    }
  }

  encoderValue = newEncoderValue;

  handleBeep();
  blinkStrip();
}

void increaseBlinkDelay() {
  currentBlinkDelayIndex++;
  if (currentBlinkDelayIndex >= 10) {
    currentBlinkDelayIndex = 9;
  }
}

void decreaseBlinkDelay() {
  currentBlinkDelayIndex--;
  if (currentBlinkDelayIndex < 0) {
    currentBlinkDelayIndex = 0;
  }
}

void printBlinkDelay() {
  Serial.print("Blink Delay: ");
  Serial.print(blinkDelays[currentBlinkDelayIndex]);
  Serial.println(" ms");
}

void setLEDColor() {
  int blinkDelay = blinkDelays[currentBlinkDelayIndex];
  blink_enabled = 0;
  
  if (blinkDelay <= 25) {
    fill_solid(leds, numLeds, CRGB::Red);
    stripBlinkDuration = 50;
    blink_enabled = 1;
  } else if (blinkDelay <= 50) {
    fill_solid(leds, numLeds, CRGB::Red);
    stripBlinkDuration = 100;
    blink_enabled = 1;
  } else if (blinkDelay <= 100) {
    fill_solid(leds, numLeds, CRGB::Red);
  } else if (blinkDelay <= 250) {
    fill_solid(leds, numLeds, CRGB::Orange);
  } else if (blinkDelay <= 500) {
    fill_solid(leds, numLeds, CRGB::Yellow);
  } else {
    fill_solid(leds, numLeds, CRGB::Green);
  }
  
  FastLED.show();
}

void handleBeep() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis < blinkDuration) {
    digitalWrite(ledPin, HIGH);
  } else if (currentMillis - previousMillis >= blinkDuration && currentMillis - previousMillis < blinkDelays[currentBlinkDelayIndex]) {
    digitalWrite(ledPin, LOW);
  } else if (currentMillis - previousMillis >= blinkDelays[currentBlinkDelayIndex]) {
    previousMillis = currentMillis;
  }
}

void blinkStrip() {
  unsigned long currentMillis = millis();

  if (currentMillis - stripPreviousMillis >= stripBlinkDuration) {
    stripPreviousMillis = currentMillis;

    // Break if blink is not enabled
    if (blink_enabled == 0) {
      FastLED.setBrightness(brightness);
      FastLED.show();
      return;
    }
    
    if (FastLED.getBrightness() > 0) {
      FastLED.setBrightness(0); // Set the brightness to 0 (turn off the LEDs)
    } else {
      FastLED.setBrightness(brightness); // Set the brightness back to the desired level
    }
    
    FastLED.show();
  }
}
