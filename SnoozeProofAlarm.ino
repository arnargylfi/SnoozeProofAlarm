
#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.   
#include "pitches.h"
#include <Time.h>
#include <TimeLib.h>

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
const int PulseWire = 0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int LED = 13;          // The on-board Arduino LED, close to PIN 13.
int Threshold = 200;           // Determine which Signal to "count as a beat" and which to ignore.
                               // Use the "Gettting Started Project" to fine-tune Threshold Value beyond default setting.
                               // Otherwise leave the default "550" value. 
int stoppingBeat = 100;
PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"
const int buzzer = 7;
volatile bool clockFlag = false; // Flag to indicate whether the clock should update
unsigned long previousMillis = 0; // Variable to store the last time the clock was updated
const unsigned long interval = 1000; // Update the clock every 1000 milliseconds

void setup() {
  int hour = 18;
  int minute = 22;   
  Serial.begin(9600);          // For Serial Monitor
  setTime(hour,minute,50,0,0,0);  

  // Configure the PulseSensor object, by assigning our variables to it. 
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);
  // Double-check the "pulseSensor" object was created and "began" seeing a signal. 
pulseSensor.begin();
}






void loop() {
int hour_vakning = 18;
int minute_vakning = 23;
  if (hour() == hour_vakning && minute()>minute_vakning-1 && minute()<minute_vakning+4){
    int myBPM = pulseSensor.getBeatsPerMinute();
    if (myBPM<stoppingBeat){
       sound();
       Serial.print("BPM: ");                        // Print phrase "BPM: " 
       Serial.println(pulseSensor.getBeatsPerMinute());
      }//Plays melody while bpm less than 100
    else{
      Serial.print("BPM: ");                        // Print phrase "BPM: " 
      Serial.println(pulseSensor.getBeatsPerMinute());                        // Print the value inside of myBPM. 
      Serial.println("Stoppa hljóð");  
      delay(60000); //Delay for one minute
      }
    
} else {
    digitalClockDisplay();
  }
}

void sound(){
  for (int thisNote = 0; thisNote < length; thisNote++) {
        // to calculate the note duration, take one second divided by the note type.
        //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
          int noteDuration = 1000 / noteDurations[thisNote];
          tone(buzzer, melody[thisNote], noteDuration);
          // to distinguish the notes, set a minimum time between them.
          // the note's duration + 30% seems to work well:
          int pauseBetweenNotes = noteDuration * 1.30;
          delay(pauseBetweenNotes);
          // stop the tone playing:
          noTone(buzzer);
    }
}

void digitalClockDisplay()
{
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println("");
  delay(1000);
}
void printDigits(int digits)
{
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);  
}




  
