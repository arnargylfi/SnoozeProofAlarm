
#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.   
#include "pitches.h"
#include <Time.h> //Time without RTC
#include <TimeLib.h> 
#include <ezBuzzer.h>


// notes in the melody:
int melody[] = {
  //Saltkjöt og baunir
   NOTE_C8, NOTE_G7, NOTE_G7, NOTE_A7, NOTE_G7, 0, NOTE_B7, NOTE_C8,NOTE_D8,NOTE_C8,NOTE_D8,NOTE_C8,NOTE_D8,NOTE_C8 
  //Pulse
  //NOTE_C4,NOTE_C4,NOTE_C4,NOTE_C4,NOTE_C4,NOTE_C4,NOTE_C4,NOTE_C4,NOTE_C4,NOTE_C4,NOTE_C4,NOTE_C4,NOTE_C4,NOTE_C4,
  
};
int length = sizeof(melody)/sizeof(int); //How many notes there are in the melody

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  //Saltkjöt og baunir
  4, 8, 8, 4, 4, 4, 4, 16,16,16,16,16,16,2 
  //Pulse
  //8,8,8,8,8,8,8,8,8,8,8,8,8,8
};

//  Variables
const int PulseWire = A5;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int LED = 13;          // The on-board Arduino LED, close to PIN 13.
int Threshold = 100;           // Determine which Signal to "count as a beat" and which to ignore.
unsigned long lastTime = 0; //For timkeeping for the clock display
int stoppingBeat = 200;
PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"
const int buzzerPin = 10;
ezBuzzer buzzer(buzzerPin);


void setup() {
  int hour = 18; //Calibrate clock, current hour
  int minute = 22;  //Calibrate cock, current minute
  Serial.begin(9600);          // For Serial Monitor
  setTime(hour,minute,50,0,0,0);  //Date is uneccessary

  // Configure the PulseSensor object, by assigning our variables to it. 
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);
  // Double-check the "pulseSensor" object was created and "began" seeing a signal. 
pulseSensor.begin();
}


void loop() {
  buzzer.loop();
  int hour_vakning = 18; //hour you want to wake up
  int minute_vakning = 23; //minute you want to wake up
  digitalClockDisplay(); //Display the time
  if (hour() == hour_vakning && minute()>minute_vakning-1 && minute()<minute_vakning+4){
    int myBPM = pulseSensor.getBeatsPerMinute();
    if (myBPM<stoppingBeat){
      //  playMelody();
      if (buzzer.getState() == BUZZER_IDLE) {
        int length = sizeof(noteDurations) / sizeof(int);
        buzzer.playMelody(melody, noteDurations, length); // playing
        Serial.print("BPM: ");                        // Print phrase "BPM: " 
        Serial.println(pulseSensor.getBeatsPerMinute());
        }
      }//Plays melody while bpm less than 100
    else{
      Serial.print("BPM: ");                        // Print phrase "BPM: " 
      Serial.println(pulseSensor.getBeatsPerMinute());                        // Print the value inside of myBPM. 
      Serial.println("Stoppa hljóð");  
      digitalClockDisplay();
      delay(4*60*1000);
     }
  }
}

// void playMelody() {
//     for (int thisNote = 0; thisNote < length; thisNote++) {
//         // to calculate the note duration, take one second divided by the note type.
//         //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
//           int noteDuration = 1000 / noteDurations[thisNote];
//           tone(buzzerPin, melody[thisNote], noteDuration);
//           // to distinguish the notes, set a minimum time between them.
//           // the note's duration + 30% seems to work well:
//           int pauseBetweenNotes = noteDuration * 1.30;
//           delay(pauseBetweenNotes);
//           // stop the tone playing:
//           noTone(buzzerPin);
//     }
// }

//Display the time as unblocking code. 
void digitalClockDisplay() {
  unsigned long currentTime = millis();

  if (currentTime - lastTime >= 1000) {
    lastTime = currentTime;

    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.println("");
  }
}
void printDigits(int digits)
{
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);  
}




  
