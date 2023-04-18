#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.   
#include "pitches.h"
#include <ezBuzzer.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
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




// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
const int PulseWire = A5;

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
int Threshold = 100;           // Determine which Signal to "count as a beat" and which to ignore.
int stoppingBeat = 400;
PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"
const int buzzerPin = 10;
ezBuzzer buzzer(buzzerPin);


#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

/******************* UI details */
#define BUTTON_X 40
#define BUTTON_Y 150
#define BUTTON_W 60
#define BUTTON_H 30
#define BUTTON_SPACING_X 20
#define BUTTON_SPACING_Y 20
#define BUTTON_TEXTSIZE 2


// text box where numbers go
#define TEXT_X 10
#define TEXT_Y 10
#define TEXT_W 220
#define TEXT_H 70
#define TEXT_TCOLOR ILI9341_GREEN
#define TEXT_LEN 5
// the data (phone #) we store in the textfield
// box where alarm goes
int ALARM_X = TEXT_X;
int ALARM_Y = TEXT_Y+TEXT_H+10;
int ALARM_W  = TEXT_W;
int ALARM_H  = TEXT_H;



#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 100
#define TS_MAXX 920

#define TS_MINY 70
#define TS_MAXY 900
// We have a status line for like, is FONA working
#define STATUS_X 10
#define STATUS_Y 65
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
Adafruit_GFX_Button buttons[10];
/* create 10 buttons, in classic candybar phone style */
char buttonlabels[10][5] = {"1", "2", "3", "4", "5", "6", "7", "8", "9",  "0" };
uint16_t buttoncolors[10] = {ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, 
                             ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, 
                             ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, 
                             ILI9341_BLUE};

bool Hjarta = false;
void setup(void) {
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.setThreshold(Threshold);
  // Double-check the "pulseSensor" object was created and "began" seeing a signal. 
  pulseSensor.begin();
  Serial.begin(9600);
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  alarmClock();
  tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, ILI9341_WHITE); //DRAW FRAME FOR CLOCK
}
// Print something in the mini status bar with either flashstring
int currentPage = 0; // 0 is the homescreen 
int wakeHour = 10;
int wakeMinute = 0;
int wakeSecond = 0;
int alarmnum[6] = {0,0,0,0,0,0};
char timeString[9];
#define MINPRESSURE 10
#define MAXPRESSURE 1000
unsigned long lastTime = 0; //For timkeeping for the clock display
int textfield_i = 0;
int textfield[6] = {0,0,0,0,0,0}; //initiating for textdisplay
void loop(void) {
  TSPoint touch = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (touch.z > MINPRESSURE && touch.z < MAXPRESSURE) {
    touch.x = map(touch.x, TS_MINX, TS_MAXX, tft.width(), 0);
    touch.y = (tft.height()-map(touch.y, TS_MINY, TS_MAXY, tft.height(), 0));
   }
  if (currentPage == 0) { //If we're on the home screen
    homeClock();
    hjartaMaeling();
    if (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE) { //check if a touch event has occurred
      if (touch.x >= TEXT_X && touch.x < (TEXT_X + TEXT_W) && touch.y >= TEXT_Y && touch.y < (TEXT_Y + TEXT_H)) { //Check if clicked the current time clock
        textfield_i = 0;
        tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, MAGENTA);
        delay(250);
        tft.fillScreen(BLACK);
        tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H+50, WHITE);
        tft.setCursor(TEXT_X + 2, TEXT_Y+5);
        tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
        tft.setTextSize(3); 
        tft.println("Stilla tima");
        createButtons();
        tft.setCursor(0, 0);// Set the cursor position
        int textfield[6] = {0,0,0,0,0,0};
        sprintf(timeString,"%02d:%02d:%02d", 00, 00, 00);
        tft.setTextSize(4);
        tft.setCursor(TEXT_X+2,TEXT_Y+30);
        tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
        tft.println(timeString);        
        currentPage = 1;
      }
      if (touch.x >= ALARM_X && touch.x < (ALARM_X + ALARM_W) && touch.y >= ALARM_Y && touch.y < (ALARM_Y + ALARM_H)) { //Check if clicked the Alarm clock
        tft.drawRect(ALARM_X, ALARM_Y, ALARM_W, ALARM_H, MAGENTA);
        textfield_i = 0;
        delay(250);
        tft.fillScreen(BLACK);
        tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H+50, WHITE);
        tft.setCursor(TEXT_X + 2, TEXT_Y+5);
        tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
        tft.setTextSize(3); 
        tft.println("Wake up time");
        createButtons();
        tft.setCursor(0, 0);// Set the cursor position
        int alarmnum[6] = {0,0,0,0,0,0};
        sprintf(timeString,"%02d:%02d:%02d", 00, 00, 00);
        tft.setTextSize(4);
        tft.setCursor(TEXT_X+2,TEXT_Y+30);
        tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
        tft.println(timeString);        
        currentPage = 2;
      }
    }
  }
  if (currentPage == 1) { //Calibrate current time page.
    if (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE) { //check if a touch event has occurred
      for (uint8_t b=0; b<9; b++) { //Go through all buttons to see if pressed
        if (buttons[b].contains(touch.x, touch.y)) {
          buttons[b].press(true);  // tell the button it is pressed
      } else {
        buttons[b].press(false);  // tell the button it is NOT pressed
      }
    }
      // now we can ask the buttons if their state has changed
      for (uint8_t b=0; b<9; b++) {
        if (buttons[b].justReleased()) {
          buttons[b].drawButton(false);  // draw normal
        }
        if (buttons[b].isPressed()) {
            buttons[b].drawButton(true);  // draw invert!
            delay(100);
            if(b<9){
              if(textfield_i==TEXT_LEN){ //return to homepage
                textfield[textfield_i] = atoi(buttonlabels[b]);
                delay(200);
                tft.fillScreen(BLACK);
                tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H+50, WHITE);
                alarmClock();
                setTime(textfield[0]*10+textfield[1],textfield[2]*10+textfield[3],textfield[4]*10+textfield[5],0,0,0);
                int textfield[6] = {0,0,0,0,0,0};
                currentPage =0;
              }
              else{
                textfield[textfield_i] = atoi(buttonlabels[b]);
                textfield_i++;
              }
            }
          char timeString[9];
          sprintf(timeString,"%02d:%02d:%02d", textfield[0]*10+textfield[1], textfield[2]*10+textfield[3], textfield[4]*10+textfield[5]);
          tft.setTextSize(4);
          tft.setCursor(TEXT_X+2,TEXT_Y+30);
          tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
          tft.println(timeString); 
        }
      }
    }  
  }
  if (currentPage == 2) { //Calibrate current time page.
  if (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE) { //check if a touch event has occurred
    for (uint8_t b=0; b<9; b++) { //Go through all buttons to see if pressed
      if (buttons[b].contains(touch.x, touch.y)) {
        buttons[b].press(true);  // tell the button it is pressed
    } else {
      buttons[b].press(false);  // tell the button it is NOT pressed
    }
  }
    // now we can ask the buttons if their state has changed
    for (uint8_t b=0; b<9; b++) {
      if (buttons[b].justReleased()) {
        buttons[b].drawButton(false);  // draw normal
      }
      if (buttons[b].isPressed()) {
          buttons[b].drawButton(true);  // draw invert!
          delay(100);
          if(b<9){
            if(textfield_i==TEXT_LEN){ //return to homepage
              alarmnum[textfield_i] = atoi(buttonlabels[b]);
              delay(200);
              tft.fillScreen(BLACK);
              tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H+50, WHITE);
              wakeHour = alarmnum[0]*10+alarmnum[1];
              wakeMinute = alarmnum[2]*10+alarmnum[3];
              wakeSecond  =alarmnum[4]*10+alarmnum[5];
              alarmClock();
              int textfield[6] = {0,0,0,0,0,0};
              currentPage =0;              
            }
            else{
              alarmnum[textfield_i] = atoi(buttonlabels[b]);
              textfield_i++;
            }
          }
        char timeString[9];
        sprintf(timeString,"%02d:%02d:%02d", alarmnum[0]*10+alarmnum[1], alarmnum[2]*10+alarmnum[3], alarmnum[4]*10+alarmnum[5]);
        tft.setTextSize(4);
        tft.setCursor(TEXT_X+2,TEXT_Y+30);
        tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
        tft.println(timeString); 
      }
    }
  }  
}   
    buzzer.loop();
    if (hour() == wakeHour && minute()==wakeMinute && second() >wakeSecond&&!Hjarta){

      if (buzzer.getState() == BUZZER_IDLE) {
        int length = sizeof(noteDurations) / sizeof(int);
        buzzer.playMelody(melody, noteDurations, length); // playing
      }
      if (pulseSensor.sawStartOfBeat()){
      int BPM = pulseSensor.getBeatsPerMinute();
        if(BPM>stoppingBeat){
        tft.setCursor(ALARM_X,ALARM_Y+ALARM_H+20);
        tft.println("Stoppa hljóð");  
        Hjarta = true;}
    }
  }
}



void hjartaMaeling(){
  if (pulseSensor.sawStartOfBeat()) {            // Constantly test to see if "a beat happened".
    int myBPM = pulseSensor.getBeatsPerMinute();  // Calls function on our pulseSensor object that returns BPM as an "int".
                                               // "myBPM" hold this BPM value now. 
    Serial.println("♥  A HeartBeat Happened ! "); // If test is "true", print a message "a heartbeat happened".
    Serial.print("BPM: ");                        // Print phrase "BPM: " 
    Serial.println(myBPM);
    tft.setCursor(ALARM_X,ALARM_Y+ALARM_H+20);
    tft.print("BPM =");
    tft.println(myBPM);
                        // Print the value inside of myBPM. 
  }
}


void alarmClock(){
    tft.drawRect(ALARM_X, ALARM_Y, ALARM_W,ALARM_H,WHITE);
    char alarmString[9]; // buffer to hold the time string
    sprintf(alarmString, "%02d:%02d:%02d", wakeHour, wakeMinute, wakeSecond);
    // update the current text field
    tft.setCursor(ALARM_X + 2, ALARM_Y+5);
    tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
    tft.setTextSize(3);
    tft.println("Wake up time");
    tft.setTextSize(4);
    tft.setCursor(ALARM_X+2,ALARM_Y+30);
    tft.println(alarmString);
  
}

void homeClock() {
  static unsigned long lastTime = 0; //For timkeeping for the clock display
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 1000) {
    lastTime = currentTime;
    
    // format the time as a string
    char timeString[9]; // buffer to hold the time string
    sprintf(timeString, "%02d:%02d:%02d", hour(), minute(), second());
    // update the current text field
    tft.setCursor(TEXT_X + 2, TEXT_Y+5);
    tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
    tft.setTextSize(3);
    tft.println("Current time");
    tft.setTextSize(4);
    tft.setCursor(TEXT_X+2,TEXT_Y+30);
    tft.println(timeString);
  }
}
void printDigits(int digits)
{
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);  
}

  // create buttons
  void createButtons(){
  bool breakOuterLoop = false;
  for (uint8_t row=0; row<4; row++) { 
    for (uint8_t col=0; col<3; col++) {
      if(col+row*3>9){  //Break out of loop if we exceed index number of buttons
        breakOuterLoop = true;
        break;
      }
      buttons[col + row*3].initButton(&tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
                BUTTON_Y+row*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
                  BUTTON_W, BUTTON_H, ILI9341_WHITE, buttoncolors[col+row*3], ILI9341_WHITE,
                  buttonlabels[col + row*3], BUTTON_TEXTSIZE); 
      buttons[col + row*3].drawButton();
    }
    if (breakOuterLoop){  //Break out of outer loop if we exceed index number of buttons
      break;
    }
  }
}




