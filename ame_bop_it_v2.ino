/*
 *  Code by Melissa Cravener for AME project 3 w/ Genesis Garibaldi.
 *  
 *  This code is for a 'BOP-IT' which is a game with various sensors that need to be pressed as they are called out by the system. Our version functions
 *  in a similar way. There are three sensors, joystick, rotary encoder, and a push button. The code calls out a random action, and the player has an increasingly short
 *  amount of time (.05 times faster each round) to complete that action or else it is over, and the game resets. Our circuit also consists of components for audio, as well as an LCD screen, in order
 *  to assist with direction giving and style of the game. A different audio cue will play for each sensor, as well being written on the screen. There is also a simple
 *  game ending sequence as well as a starting section.
 * 
 * Credit to https://mhsp18object.wordpress.com/2018/05/06/project-2-diy-arduino-sensor-bop-it/ for parts of the code.
 */

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(22, 14, 23, 36, 15, 16);    // these numbers correlate to their pins: RS, E, D4, D5, D6, D7

// GUItool: begin automatically generated code, for audio
AudioSynthKarplusStrong  string2;        //xy=535,548
AudioSynthWaveformSine   sine2;          //xy=536.6666870117188,468.00006103515625
AudioSynthWaveformSine   sine3;          //xy=537,508
AudioSynthKarplusStrong  string1;        //xy=754.6666870117188,372
AudioMixer4              mixer2;         //xy=755,498
AudioSynthSimpleDrum     drum1;          //xy=755.6666870117188,335
AudioSynthWaveformSine   sine1;          //xy=757.6667060852051,407.0000114440918
AudioMixer4              mixer1;         //xy=903.0001220703125,384.0000114440918
AudioOutputAnalog        dac1;           //xy=1039.666648864746,383.00001335144043
AudioConnection          patchCord1(string2, 0, mixer2, 2);
AudioConnection          patchCord2(sine2, 0, mixer2, 0);
AudioConnection          patchCord3(sine3, 0, mixer2, 1);
AudioConnection          patchCord4(string1, 0, mixer1, 1);
AudioConnection          patchCord5(mixer2, 0, mixer1, 3);
AudioConnection          patchCord6(drum1, 0, mixer1, 0);
AudioConnection          patchCord7(sine1, 0, mixer1, 2);
AudioConnection          patchCord8(mixer1, dac1);
// GUItool: end automatically generated code

//sensor values
int X_pin = 20;
int encoder0PinA = 19;
int encoder0PinB = 18;
int buttonpin = 17;

//game values
long action;     // shows the current action
int score = 0;   // the current score
int topscore = 0; //the topscore
bool actionwon;  // was the action completed
bool gameon;     //is the game currently being played
// each sensor gets two ints for the testing
int rotary1;
int rotary2;
int joystick1;
int joystick2;
bool button1;
bool button2;

int encoder0Pos = 0; //stuff for the rotary to sense properly
int encoder0PinALast = LOW;
int n = LOW;
unsigned long gameSpeed = 3000; //stuff for the timers
unsigned long timeOfPrompt;
unsigned long timeElapsed;


 void setup() {
  Serial.begin(9600);
  pinMode (encoder0PinA, INPUT);
  pinMode (encoder0PinB, INPUT);
  pinMode (X_pin, INPUT);
  pinMode(buttonpin, INPUT); 
  AudioMemory(10); 
  lcd.begin(16, 2);
  randomSeed(analogRead(A18));
}

void loop() {
  start_game();
  gameon = true;
  while (gameon != false) {
    action = random(1,4); //creates random action
    actionwon = false;
    
    if (action == 1){         //1 = the button
      
      lcd.print("PUSH IT"); 
      button1 = digitalRead(buttonpin);
      push_sound();
      //timer starts
      timeOfPrompt = millis();
      timeElapsed = millis() - timeOfPrompt;

        //while you still have time, and havent done the action yet
        while (timeElapsed < gameSpeed && actionwon == false) {
          button2 = digitalRead(buttonpin);
          if (button2 != button1){   //the status of the sensor has changed, action was done
            actionwon = true;
            score = score + 1;  //updates score
            gameSpeed = gameSpeed - (gameSpeed * .05);   //increases speed
          }
          timeElapsed = millis() - timeOfPrompt;
        }
        if (actionwon == true){    //won round, continues to next
          lcd.clear();
          lcd.print("   ");
          delay(1000);
        }
        else if(actionwon == false){    // lost round, starts over b/c endgame is false, leaves the while
          gameon = false;
          end_game();
        }
        lcd.clear();
    }
    
    if (action == 2){         //2 = the joystick

      //code is the same for three sensors, except for it reading the specific sensor
      
      lcd.print("FLICK IT");
      flick_sound();
      joystick1 = digitalRead(X_pin);
      timeOfPrompt = millis();
      timeElapsed = millis() - timeOfPrompt;
        while (timeElapsed < gameSpeed && actionwon == false) {
          joystick2 = digitalRead(X_pin);
          if (joystick2 != joystick1){
            actionwon = true;
            score = score + 1;
            gameSpeed = gameSpeed - (gameSpeed * .05);
          }
          timeElapsed = millis() - timeOfPrompt;
        }
        if (actionwon == true){
          lcd.clear();
          lcd.print("   ");
          delay(1000);
        }
        else if(actionwon == false){
          gameon = false;
          end_game();
        }
        lcd.clear();
    }
    
    if (action == 3){     //3 = the rotary encoder
      
      lcd.print("SPIN IT");
      spin_sound();
        n = digitalRead(encoder0PinA);
        if ((encoder0PinALast == LOW) && (n == HIGH)) {
        if (digitalRead(encoder0PinB) == LOW) {
          encoder0Pos--;
        } else {
          encoder0Pos++; }}
      encoder0PinALast = n;
      rotary1 = n;

      timeOfPrompt = millis();
      timeElapsed = millis() - timeOfPrompt;
        while (timeElapsed < gameSpeed && actionwon == false) {
            n = digitalRead(encoder0PinA);
            if ((encoder0PinALast == LOW) && (n == HIGH)) {
            if (digitalRead(encoder0PinB) == LOW) {
              encoder0Pos--;
             } else {
            encoder0Pos++;}}
            encoder0PinALast = n;
            rotary2 = n;
                      
          if (rotary2 != rotary1){
            actionwon = true;
            score = score + 1;
            gameSpeed = gameSpeed - (gameSpeed * .05);
          }
          timeElapsed = millis() - timeOfPrompt;
        }
        if (actionwon == true){
          lcd.clear();
          lcd.print("  ");
          delay(1000);
        }
        else if (actionwon == false){
          gameon = false;
          end_game();
        }
        lcd.clear();
    }

  }
  //game rests, sets top score
  if (score > topscore){
    topscore = score; }
   gameSpeed = 3000;
   score = 0;
}
void start_game(){
  //runs at the start of the loop, just visuals/audio to look nice
  lcd.print("starting...");
  sine1.amplitude(1.0);
  sine1.frequency(400);
  delay(300);
  sine1.amplitude(0);
  delay(700);
  lcd.setCursor(0,1);
  lcd.print("3   ");
  sine1.amplitude(1.0);
  sine1.frequency(400);
  delay(300);
  sine1.amplitude(0);
  delay(700);  
  lcd.print("2   ");
  sine1.amplitude(1.0);
  sine1.frequency(400);
  delay(300);
  sine1.amplitude(0);
  delay(700);
  lcd.print("1   ");
  sine1.amplitude(1.0);
  sine1.frequency(800);
  delay(300);
  sine1.amplitude(0);
  delay(700);
  lcd.clear();
  lcd.setCursor(0,0);  
  
}
void end_game(){
  //runs after a failed round, shows final score
  sine1.amplitude(1.0);
  sine1.frequency(250);
  sine2.amplitude(1.0);
  sine2.frequency(300);  
  sine3.amplitude(1.0);
  sine3.frequency(200);
  delay(1000);
  sine1.amplitude(0);
  sine2.amplitude(0);
  sine3.amplitude(0);  
  lcd.clear();
  lcd.print("final score:  ");
  lcd.print(score);
  lcd.setCursor(0,1);
  lcd.print("top score: ");
  lcd.print(topscore);
  lcd.setCursor(0,0);
  delay(3000);
  lcd.clear();
}

//the sounds for each action, each one is different so the player can learn to recognize them and not have to look at the screen anymore.

void push_sound(){
  sine1.amplitude(0.7);
  sine2.amplitude(0.7);
  sine3.amplitude(0.7);
  sine1.frequency(500);
  sine2.frequency(550);
  sine3.frequency(700);
  delay(200);
  sine1.amplitude(0);
  sine2.amplitude(0);
  sine3.amplitude(0);
}
void flick_sound(){
  string1.noteOn(300, 1);
  string2.noteOn(200, 1);
  delay(400);
  string1.noteOff(1);
  string2.noteOff(1);
}
void spin_sound(){
  drum1.noteOn();
  drum1.frequency(600);
  drum1.length(800);  
}
