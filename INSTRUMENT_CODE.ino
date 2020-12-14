#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <NewPing.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define TRIGGER_PIN  22  // trigger pin - ultrasonic sensor. 
#define ECHO_PIN     23  // echo pin - ultrasonic sensor.  
#define MAX_DISTANCE 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.  
#define NEOPIXEL_PIN 16  // TEENSY TO NEOPIXEL CONNECTION
#define NUMPIXELS    24  // NUMBER OF PIXELS

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);        //NEED TO EDIT NEOPIX PIN TO WHATEVER PIN WE USED
 
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
int pingout; //for us

elapsedMillis myTimer01;
int myTimer01Interval = 500;

const int X_pin = A16; // analog pin connected to X output - joystick
const int Y_pin = A15; // analog pin connected to Y output

int Xnew;
int Ynew;

// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine3;          //xy=512.3333282470703,422.0000114440918
AudioSynthWaveformSine   sine1;          //xy=524.000114440918,320.0000104904175
AudioSynthWaveformSine   sine2;          //xy=524.9999923706055,482.00001430511475
AudioMixer4              mixer1;         //xy=649.9999961853027,442.0000114440918
AudioMixer4              mixer2;         //xy=789.3333358764648,345.00000953674316
AudioOutputAnalogStereo  dacs1;          //xy=937.0000057220459,340.00000953674316
AudioConnection          patchCord1(sine3, 0, mixer1, 0);
AudioConnection          patchCord2(sine1, 0, mixer2, 0);
AudioConnection          patchCord3(sine2, 0, mixer1, 3);
AudioConnection          patchCord4(mixer1, 0, mixer2, 3);
AudioConnection          patchCord5(mixer2, 0, dacs1, 0);
AudioConnection          patchCord6(mixer2, 0, dacs1, 1);
// GUItool: end automatically generated code

void setup() {
  AudioMemory(20); 
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setBrightness(160);
}

void loop() {

 if (myTimer01 > myTimer01Interval) {
  
  //ULTRASONIC READ
  unsigned int uS = sonar.ping();
  pingout = uS / US_ROUNDTRIP_CM;
  
  //JOYSTICK
  Xnew = analogRead(X_pin);
  Ynew = analogRead(Y_pin);
  int Xsine = map(Xnew,0,1000,200,800);
  int Ysine = map(Ynew,0,1000,200,800);
    
  //NOISE MAKING PART
  sine1.amplitude(1.0);
  sine2.amplitude(1.0);
  sine3.amplitude(1.0);
  int sine = map(pingout,0,150,800,200);
  sine1.frequency(sine);
  sine2.frequency(Xsine);
  sine3.frequency(Ysine);
  mixer1.gain(0, 0.5);
  mixer1.gain(1, 0.5);

  //NEOPIXEL
  int red = map(sine,800,200,0,255);
  int green = map(Xsine,200,800,0,255);
  int blue = map(Ysine,200,800,0,255);
   for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(red,green,blue));
      pixels.show(); // This sends the updated pixel color to the hardware.
     }
   myTimer01 = 0; // reset our timer
   }
 }
